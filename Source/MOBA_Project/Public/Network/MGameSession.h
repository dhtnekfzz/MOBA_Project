// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "MGameSession.generated.h"

UCLASS()
class MOBA_PROJECT_API AMGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	virtual bool ProcessAutoLogin() override;
	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite) override;
	virtual void UnregisterPlayer(FName FromSessionName, const FUniqueNetIdRepl& UniqueId) override;
};
