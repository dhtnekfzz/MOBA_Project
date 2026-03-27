// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/MNetStatics.h"

#include "GameFramework/GameSession.h"

FOnlineSessionSettings UMNetStatics::GenerateOnlineSessionSettings(const FName& SessionName,const FString& SessionSearchId, int Port)
{
	FOnlineSessionSettings OnlineSessionSettings{};
	OnlineSessionSettings.bIsLANMatch = true;
	OnlineSessionSettings.NumPublicConnections = GetPlayerCountPerTeam() * 2;
	OnlineSessionSettings.bShouldAdvertise = true;
	OnlineSessionSettings.bUsesPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	OnlineSessionSettings.bAllowInvites = true;
	OnlineSessionSettings.bAllowJoinInProgress = false;
	OnlineSessionSettings.bUseLobbiesIfAvailable = false;
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	OnlineSessionSettings.bUsesStats = true;

	OnlineSessionSettings.Set(GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetSessionSearchIdKey(), SessionSearchId, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetPortKey(), Port, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	return OnlineSessionSettings;
}

IOnlineSessionPtr UMNetStatics::GetSessionPtr()
{
	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem)
	{
		return OnlineSubSystem->GetSessionInterface();
	}
	return nullptr;
}

IOnlineIdentityPtr UMNetStatics::GetIdentityPtr()
{
	IOnlineSubsystem* OnlineSubSystem = IOnlineSubsystem::Get();
	if (OnlineSubSystem)
	{
		return OnlineSubSystem->GetIdentityInterface();
	}
	return nullptr;
}

uint8 UMNetStatics::GetPlayerCountPerTeam()
{
	return 5;
}

bool UMNetStatics::IsSessionServer(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer;
}

FString UMNetStatics::GetSessionNameStr()
{
	return GetCommandlineArgAsString(GetSessionNameKey());
}

FName UMNetStatics::GetSessionNameKey()
{
	return FName("SESSION_NAME");
}

FString UMNetStatics::GetSessionSearchIdStr()
{
	return GetCommandlineArgAsString(GetSessionSearchIdKey());
}

FName UMNetStatics::GetSessionSearchIdKey()
{
	return FName("SESSION_SEARCH_ID");
}

int UMNetStatics::GetSessionPort()
{
	return GetCommandlineArgAsInt(GetPortKey());
}

FName UMNetStatics::GetPortKey()
{
	return FName("PORT");
}

FName UMNetStatics::GetCoordinatorURLKey()
{
	return FName("COORDINATOR_URL");
}

FString UMNetStatics::GetCoordinatorURL()
{
	FString CoordinatorURL = GetCommandlineArgAsString(GetCoordinatorURLKey());
	if (CoordinatorURL != "")
	{
		return CoordinatorURL;
	}

	return GetDefaultCoordinatorURL();
}

FString UMNetStatics::GetDefaultCoordinatorURL()
{
#if WITH_EDITOR
	// 에디터: DS 내장 HTTP 서버(localhost:3000)로 직접 연결
	return TEXT("http://127.0.0.1:3000");
#else
	FString CoordinatorURL = "";
	GConfig->GetString(TEXT("MOBA.Net"), TEXT("CoordinatorURL"), CoordinatorURL, GGameIni);
	UE_LOG(LogTemp, Warning, TEXT("Getting Default Coordinator URL as: %s"), *CoordinatorURL)
	return CoordinatorURL;
#endif
}

FString UMNetStatics::GetCommandlineArgAsString(const FName& ParamName)
{
	FString OutVal = "";
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int UMNetStatics::GetCommandlineArgAsInt(const FName& ParamName)
{
	int OutVal = 0;
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

FString UMNetStatics::GetTestingURL()
{
	FString TestURL = GetCommandlineArgAsString(GetTestingURLKey());
	UE_LOG(LogTemp, Warning, TEXT("Get Testing URL: %s"), *TestURL)
	return TestURL;
}

FName UMNetStatics::GetTestingURLKey()
{
	return FName("TESTING_URL");
}

void UMNetStatics::ReplacePort(FString& OutURLStr, int NewPort)
{
	FURL URL(nullptr, *OutURLStr, ETravelType::TRAVEL_Absolute);
	URL.Port = NewPort;
	OutURLStr = URL.ToString();
}
