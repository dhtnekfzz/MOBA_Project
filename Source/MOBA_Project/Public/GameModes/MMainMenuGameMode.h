// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MMainMenuGameMode.generated.h"


UCLASS()
class MOBA_PROJECT_API AMMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMMainMenuGameMode();

	virtual void RestartPlayer(AController* NewPlayer) override;
	
};
