// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpResultCallback.h"
#include "IHttpRouter.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/IHttpRequest.h"
#include "MGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnMLoginCompleted,bool /*bWasSuccessful*/, const FString& /*PlayerNickName*/, const FString& /*ErrorMsg*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMSessionSearchCompleted,const TArray<FOnlineSessionSearchResult>& /*SearchResults*/);
DECLARE_MULTICAST_DELEGATE(FOnMJoinSessionFailed);

UCLASS()
class MOBA_PROJECT_API UMGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	void StartMatch();

	void PlayerJoined(const FUniqueNetIdRepl& UniqueId);
	void PlayerLeft(const FUniqueNetIdRepl& UniqueId);

	bool IsLoggedIn() const;
	bool IsLoggingIn() const;
	void ClientAccountPortalLogin();
	FOnMLoginCompleted OnLoginCompleted;

	// 에디터 로그인
	void EditorLoginWithNickname(const FString& Nickname);
	FORCEINLINE FString GetPendingNickName() const { return PendingNickname; }
	
	void RequestCreateAndJoinSession(const FName& NewSessionName);
	void CancelSessionCreation();
	void StartGlobalSessionSearch();
	bool JoinSessionWithId(const FString& SessionIdStr);

	FOnMSessionSearchCompleted OnGlobalSessionSearchCompleted;
	FOnMJoinSessionFailed OnJoinSessionFailed;

private:
	void ClientLogin(const FString& Type, const FString& Id, const FString& Token);
	void LoginCompleted(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	FDelegateHandle LoggingInDelegateHandle;
	FString PendingNickname;

	void FindGlobalSessions();
	void GlobalSessionSearchCompleted(bool bWasSuccessful);
	void FindCreatedSession(FGuid SessionSearchId);
	void FindCreateSessionCompleted(bool bWasSuccessful);
	void FindCreatedSessionTimeout();
	void SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully, FGuid SessionSearchId);
	void StartFindingCreatedSession(const FGuid& SessionSearchId);
	void StopAllSessionFindings();
	void StopFindingCreatedSession();
	void StopGlobalSessionSearch();
	void JoinSessionWithSearchResult(const FOnlineSessionSearchResult& SearchResult);
	void JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int Port);

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FTimerHandle FindCreatedSessionTimerHandle;
	FTimerHandle FindCreatedSessionTimeoutHandle;
	FTimerHandle GlobalSessionSearchTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category="Session")
	float GlobalSessionSearchInterval = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="Session")
	float FindCreatedSessionInterval = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="Session")
	float FindCreatedSessionTimeoutDuration = 60.f;

	void StartCoordinatorServer();
	void StopCoordinatorServer();
	bool HandleSessionCreationRequest(const FHttpServerRequest& Request,const FHttpResultCallback& OnComplete);
	TSharedPtr<IHttpRouter> HttpRouter;

	UPROPERTY(EditDefaultsOnly, Category="Session")
	int32 CoordinatorPort = 3000;

	void CreateSession(const FName& SessionName,const FString& SearchId, int Port);
	void OnSessionCreated(FName SessionName, bool bWasSuccessful);
	void EndSessionCompleted(FName SessionName, bool bWasSuccessful);
	void TerminateSessionServer();
	void WaitPlayerJoinTimeoutReached();

	FString ServerSessionName;
	int ServerSessionPort = 7777;
	TSet<FUniqueNetIdRepl> PlayerRecord;

	FTimerHandle WaitPlayerJoinTimeoutHandle;

	UPROPERTY(EditDefaultsOnly, Category="Session")
	float WaitPlayerJoinTimeoutDuration = 60.f;

	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSoftObjectPtr<UWorld> MainMenuLevel;

	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UPROPERTY(EditDefaultsOnly, Category="Map")
	TSoftObjectPtr<UWorld> GameLevel;

	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);

	bool bSessionCreationInProgress=false;
};
