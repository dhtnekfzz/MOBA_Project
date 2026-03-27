// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/MGameInstance.h"

#include "HttpModule.h"
#include "HttpServerModule.h"
#include "Network/MNetStatics.h"          
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/IHttpResponse.h"
#include "OnlineSubsystem.h"

void UMGameInstance::Init()
{
	Super::Init();

	if (!UMNetStatics::IsSessionServer(this)) return;
	
	StartCoordinatorServer();
}

void UMGameInstance::Shutdown()
{
	Super::Shutdown();
	StopCoordinatorServer();
}


void UMGameInstance::StartMatch()
{
	if (GetWorld()->GetNetMode() == NM_DedicatedServer || GetWorld()->GetNetMode() == NM_ListenServer)
	{
		StopCoordinatorServer();
		LoadLevelAndListen(GameLevel);
	}
}

bool UMGameInstance::IsLoggedIn() const
{
	if (IOnlineIdentityPtr Id = UMNetStatics::GetIdentityPtr())
		return Id->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	return false;
}

bool UMGameInstance::IsLoggingIn() const
{
	return LoggingInDelegateHandle.IsValid();
}

void UMGameInstance::ClientAccountPortalLogin()
{
	ClientLogin("AccountPortal", "", "");
}

void UMGameInstance::ClientLogin(const FString& Type, const FString& Id, const FString& Token)
{
	IOnlineIdentityPtr IdentityPtr = UMNetStatics::GetIdentityPtr();
	if (!IdentityPtr) return;

	if (LoggingInDelegateHandle.IsValid())
	{
		IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
		LoggingInDelegateHandle.Reset();
	}

	LoggingInDelegateHandle = IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &UMGameInstance::LoginCompleted);

	if (!IdentityPtr->Login(0, FOnlineAccountCredentials(Type, Id, Token)))
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();
		}
		OnLoginCompleted.Broadcast(false, "", "Login Failed Immediately");
	}
}

void UMGameInstance::LoginCompleted(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	IOnlineIdentityPtr IdentityPtr = UMNetStatics::GetIdentityPtr();
	if (!IdentityPtr) { OnLoginCompleted.Broadcast(false, "", "No Identity Ptr"); return; }

	if (LoggingInDelegateHandle.IsValid())
	{
		IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
		LoggingInDelegateHandle.Reset();
	}

	FString Nickname = bWasSuccessful ? IdentityPtr->GetPlayerNickname(UserId) : "";
	OnLoginCompleted.Broadcast(bWasSuccessful, Nickname, Error);
}


void UMGameInstance::EditorLoginWithNickname(const FString& Nickname)
{
#if WITH_EDITOR
	PendingNickname=Nickname;
	OnLoginCompleted.Broadcast(true, Nickname, TEXT(""));
#endif
}

void UMGameInstance::RequestCreateAndJoinSession(const FName& NewSessionName)
{
	FGuid SearchId = FGuid::NewGuid();

	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FGuid SessionSearchID = FGuid::NewGuid();

	FString CoordinatorURL = UMNetStatics::GetCoordinatorURL();

	FString URL = FString::Printf(TEXT("%s/Sessions"), *CoordinatorURL);
	UE_LOG(LogTemp, Warning, TEXT("Sending Request Session Creation to URL: %s"), *URL)

	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(UMNetStatics::GetSessionNameKey().ToString(), NewSessionName.ToString());
	JsonObject->SetStringField(UMNetStatics::GetSessionSearchIdKey().ToString(), SessionSearchID.ToString());

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &UMGameInstance::SessionCreationRequestCompleted, SessionSearchID);
	
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("Sesison Creation Request Failed Right Away!"))
	}
}

void UMGameInstance::CancelSessionCreation()
{
	StopAllSessionFindings();
	if (IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
	StartGlobalSessionSearch();
}


void UMGameInstance::StartGlobalSessionSearch()
{
	GetWorld()->GetTimerManager().SetTimer(
		GlobalSessionSearchTimerHandle,
		this, &UMGameInstance::FindGlobalSessions,
		GlobalSessionSearchInterval, true, 0.f);
}

bool UMGameInstance::JoinSessionWithId(const FString& SessionIdStr)
{
	if (!SessionSearch.IsValid()) return false;
	const FOnlineSessionSearchResult* Result = SessionSearch->SearchResults.FindByPredicate([&](const FOnlineSessionSearchResult& R) { return R.GetSessionIdStr() == SessionIdStr; });
	if (!Result) return false;
	JoinSessionWithSearchResult(*Result);
	return true;
}

void UMGameInstance::FindGlobalSessions()
{
	IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr();
	if (!SessionPtr) return;

	if (SessionSearch.IsValid() && SessionSearch->SearchState == EOnlineAsyncTaskState::InProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("이전 Global검색이 진행 중입니다. 이번 틱은 스킵합니다."));
		return; 
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 20;

	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UMGameInstance::GlobalSessionSearchCompleted);
	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
}

void UMGameInstance::GlobalSessionSearchCompleted(bool bWasSuccessful)
{
	if (bWasSuccessful)
		OnGlobalSessionSearchCompleted.Broadcast(SessionSearch->SearchResults);

	if (IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr())
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
}

void UMGameInstance::StartFindingCreatedSession(const FGuid& SessionSearchId)
{
	if (!SessionSearchId.IsValid()) return;
	
	StopAllSessionFindings();

	GetWorld()->GetTimerManager().SetTimer(
		FindCreatedSessionTimerHandle,
		FTimerDelegate::CreateUObject(this, &UMGameInstance::FindCreatedSession, SessionSearchId),
		FindCreatedSessionInterval, true, 0.f);

	GetWorld()->GetTimerManager().SetTimer(FindCreatedSessionTimeoutHandle,this, &UMGameInstance::FindCreatedSessionTimeout,FindCreatedSessionTimeoutDuration);
}

void UMGameInstance::FindCreatedSession(FGuid SessionSearchId)
{
	IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr();
	if (!SessionPtr) return;

	if (SessionSearch.IsValid() && SessionSearch->SearchState == EOnlineAsyncTaskState::InProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("이전 검색이 진행 중입니다. 이번 틱은 스킵합니다."));
		return; 
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 1;
	SessionSearch->QuerySettings.Set(UMNetStatics::GetSessionSearchIdKey(), SessionSearchId.ToString(),EOnlineComparisonOp::Equals);

	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UMGameInstance::FindCreateSessionCompleted);
	
	if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("세션을 찾지 못했습니다."))
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UMGameInstance::FindCreateSessionCompleted(bool bWasSuccessful)
{
	if (!bWasSuccessful || SessionSearch->SearchResults.Num() == 0) return;
	
	StopFindingCreatedSession();
	UE_LOG(LogTemp, Warning, TEXT("세션을 찾았습니다! 서버가 맵 로딩을 끝낼 때까지 2초 대기합니다..."));
	
	FOnlineSessionSearchResult FoundResult = SessionSearch->SearchResults[0];

	FTimerHandle JoinDelayTimer;
	GetWorld()->GetTimerManager().SetTimer(JoinDelayTimer, FTimerDelegate::CreateLambda([this, FoundResult]()
	{
		UE_LOG(LogTemp, Warning, TEXT("서버 맵 로딩 완료 예상. 접속(JoinSession)을 시도합니다!"));
		JoinSessionWithSearchResult(FoundResult);
	}), 3.0f, false);
}

void UMGameInstance::FindCreatedSessionTimeout()
{
	UE_LOG(LogTemp, Warning, TEXT("Session Find TimeOut"));
	StopFindingCreatedSession();
	
	
}

void UMGameInstance::SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response,bool bConnectedSuccessfully, FGuid SessionSearchId)
{
	if (!bConnectedSuccessfully)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection responded with connection was not successful!"))
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Connection to Coordinator Successfully!"))

	int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Failed, with code: %d"), ResponseCode)
		return;
	}

	FString ResponseStr = Response->GetContentAsString();
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
	int32 Port = 0;

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		Port = JsonObject->GetIntegerField(*(UMNetStatics::GetPortKey().ToString()));
	}

	UE_LOG(LogTemp, Warning, TEXT("Conntected to Coordinator Successfully and the new session created is on port: %d"), Port)
	StartFindingCreatedSession(SessionSearchId);
}

void UMGameInstance::StopAllSessionFindings()
{
	StopFindingCreatedSession();
	StopGlobalSessionSearch();
}

void UMGameInstance::StopFindingCreatedSession()
{
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimeoutHandle);
	
	if (IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}

}

void UMGameInstance::StopGlobalSessionSearch()
{
	GetWorld()->GetTimerManager().ClearTimer(GlobalSessionSearchTimerHandle);
	if (IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr())
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
}

void UMGameInstance::JoinSessionWithSearchResult(const FOnlineSessionSearchResult& SearchResult)
{
	IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		OnJoinSessionFailed.Broadcast();
		return;
	}

	FString SessionName;
	SearchResult.Session.SessionSettings.Get<FString>(UMNetStatics::GetSessionNameKey(), SessionName);

	int32 Port = 7777;
	if (const FOnlineSessionSetting* PortSetting = SearchResult.Session.SessionSettings.Settings.Find(UMNetStatics::GetPortKey()))
	{
		PortSetting->Data.GetValue(Port);
	}

	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UMGameInstance::JoinSessionCompleted, Port);
	
	if (!SessionPtr->JoinSession(0, FName(SessionName), SearchResult))
	{
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		OnJoinSessionFailed.Broadcast();
	}
}

void UMGameInstance::JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int Port)
{
	IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		OnJoinSessionFailed.Broadcast();
		return;
	}

	if (JoinResult == EOnJoinSessionCompleteResult::Success)
	{
		StopAllSessionFindings();
		FString TravelURL;
		SessionPtr->GetResolvedConnectString(SessionName, TravelURL);
		
		FString CleanIP;
		FString GarbageData;
        
		TravelURL.Split(TEXT(":"), &CleanIP, &GarbageData); 

		TravelURL = FString::Printf(TEXT("%s:%d"), *CleanIP, Port);
		
		UE_LOG(LogTemp, Warning, TEXT("Join Session Complete → %s"), *TravelURL);
		GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, TRAVEL_Absolute);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Session Fail"));
		OnJoinSessionFailed.Broadcast();
	}

	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
}


void UMGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimeoutHandle);
	
	PlayerRecord.Add(UniqueId);
}

void UMGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	if (GetWorld()->IsEditorWorld()) return;
	PlayerRecord.Remove(UniqueId);
	if (PlayerRecord.Num() == 0)
		TerminateSessionServer();
}

void UMGameInstance::StartCoordinatorServer()
{
	FHttpServerModule& HttpModule = FHttpServerModule::Get();
	HttpRouter = HttpModule.GetHttpRouter(CoordinatorPort);

	if (!HttpRouter.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[DS] HTTP Router 생성 실패 (Port:%d)"), CoordinatorPort);
		return;
	}

	HttpRouter->BindRoute(FHttpPath(TEXT("/Sessions")),EHttpServerRequestVerbs::VERB_POST, FHttpRequestHandler::CreateUObject(this, &UMGameInstance::HandleSessionCreationRequest));

	HttpModule.StartAllListeners();
	UE_LOG(LogTemp, Warning, TEXT("[DS] Coordinator HTTP 서버 시작 (Port:%d) - 세션 생성 요청 대기 중"), CoordinatorPort);
}

void UMGameInstance::StopCoordinatorServer()
{
	if (FHttpServerModule::IsAvailable())
		FHttpServerModule::Get().StopAllListeners();
	
	HttpRouter.Reset();
}

bool UMGameInstance::HandleSessionCreationRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	TArray<uint8> SafeBodyBytes = Request.Body;
	SafeBodyBytes.Add(0);

	FString BodyStr = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(SafeBodyBytes.GetData())));
	
	TSharedPtr<FJsonObject> JsonBody;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BodyStr);

	if (!FJsonSerializer::Deserialize(Reader, JsonBody) || !JsonBody.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[DS] 요청 JSON 파싱 실패: %s"), *BodyStr);
		OnComplete(FHttpServerResponse::Error(EHttpServerResponseCodes::BadRequest, TEXT("Invalid JSON"), TEXT("")));
		return true;
	}

	FString SessionName = JsonBody->GetStringField(UMNetStatics::GetSessionNameKey().ToString());
	FString SearchId = JsonBody->GetStringField(UMNetStatics::GetSessionSearchIdKey().ToString());

	if (SessionName.IsEmpty() || SearchId.IsEmpty())
	{
		OnComplete(FHttpServerResponse::Error(EHttpServerResponseCodes::BadRequest,TEXT("SESSION_NAME 또는 SESSION_SEARCH_ID 누락"), TEXT("")));
		return true;
	}

	ServerSessionPort = UMNetStatics::GetSessionPort();
	ServerSessionName = SessionName;

	UE_LOG(LogTemp, Warning, TEXT("[DS] 세션 생성 요청 수신 → Name:%s / SearchId:%s / Port:%d"), *SessionName, *SearchId, ServerSessionPort);

	TSharedPtr<FJsonObject> ResponseJson = MakeShareable(new FJsonObject);
	ResponseJson->SetNumberField(UMNetStatics::GetPortKey().ToString(), ServerSessionPort);

	FString ResponseStr;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseStr);
	FJsonSerializer::Serialize(ResponseJson.ToSharedRef(), Writer);

	auto Response = FHttpServerResponse::Create(ResponseStr, TEXT("application/json"));
	OnComplete(MoveTemp(Response));
	
	FString CapturedSessionName = SessionName;
	FString CapturedSearchId = SearchId;
	int32 CapturedPort = ServerSessionPort;

	AsyncTask(ENamedThreads::GameThread, [this, CapturedSessionName,CapturedSearchId, CapturedPort]()
	{
		if (bSessionCreationInProgress) return;
		bSessionCreationInProgress = true;

		CreateSession(FName(*CapturedSessionName), CapturedSearchId, CapturedPort);
	});
	return true;
}

void UMGameInstance::CreateSession(const FName& SessionName,const FString& SearchId, int Port)
{
	IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		TerminateSessionServer();
		return;
	}

	FOnlineSessionSettings Settings = UMNetStatics::GenerateOnlineSessionSettings(SessionName, SearchId, Port);

	SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UMGameInstance::OnSessionCreated);
	if (!SessionPtr->CreateSession(0,SessionName, Settings))
	{
		UE_LOG(LogTemp, Error, TEXT("Session Create failed"));
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		TerminateSessionServer();
	}
}

void UMGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
	bSessionCreationInProgress=false;
	
	if (bWasSuccessful)
	{
		GetWorld()->GetTimerManager().SetTimer(
			WaitPlayerJoinTimeoutHandle,
			this, &UMGameInstance::WaitPlayerJoinTimeoutReached,
			WaitPlayerJoinTimeoutDuration);
		LoadLevelAndListen(LobbyLevel);
	}
	else
	{
		TerminateSessionServer();
	}

	if (IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr())
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
}

void UMGameInstance::EndSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);
}

void UMGameInstance::TerminateSessionServer()
{
	if (IOnlineSessionPtr SessionPtr = UMNetStatics::GetSessionPtr())
	{
		SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UMGameInstance::EndSessionCompleted);
		
		if (!SessionPtr->EndSession(FName(ServerSessionName)))
		{
			UE_LOG(LogTemp, Error, TEXT("Session End failed"));
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UMGameInstance::WaitPlayerJoinTimeoutReached()
{
	TerminateSessionServer();
}

void UMGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FString LevelURL = FPackageName::ObjectPathToPackageName(Level.ToString());
	if (LevelURL.IsEmpty()) return;

	FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL, ServerSessionPort);
	GetWorld()->ServerTravel(TravelStr);
}
