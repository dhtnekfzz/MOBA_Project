// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/GameModeBase.h"
#include "MGameMode.generated.h"

class ACapturePoint;

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API AMGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMGameMode();

	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	virtual void StartPlay() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

private:
	FGenericTeamId GetTeamIDForPlayer(const AController* InController) const;

	AActor* FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const;

	UPROPERTY(EditDefaultsOnly, Category="Team")
	TSubclassOf<APawn> BackupPawn;

	UPROPERTY(EditDefaultsOnly, Category="Team")
	TMap<FGenericTeamId, FName> TeamStartSpotTagMap;
	
	ACapturePoint* GetCapturePoint() const;
	void OnCapturePointVictory(int32 WinningTeamId);
	void MatchFinished(AActor* ViewTarget, int32 WinningTeamId);
};
