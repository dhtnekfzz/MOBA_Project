// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AutomationBlueprintFunctionLibrary.h"
#include "OnlineSessionSettings.h"
#include "MNetStatics.generated.h"

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FOnlineSessionSettings GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchId, int Port);

	static IOnlineSessionPtr GetSessionPtr();
	static IOnlineIdentityPtr GetIdentityPtr();

	static uint8 GetPlayerCountPerTeam();

	static bool IsSessionServer(const UObject* WorldContextObject);

	static FString GetSessionNameStr();
	static FName GetSessionNameKey();

	static FString GetSessionSearchIdStr();
	static FName GetSessionSearchIdKey();

	static int GetSessionPort();
	static FName GetPortKey();

	static FName GetCoordinatorURLKey();
	static FString GetCoordinatorURL();
	static FString GetDefaultCoordinatorURL();

	static FString GetCommandlineArgAsString(const FName& ParamName);
	static int GetCommandlineArgAsInt(const FName& ParamName);

	static FString GetTestingURL();
	static FName GetTestingURLKey();

	static void ReplacePort(FString& OutURLStr, int NewPort);
};
