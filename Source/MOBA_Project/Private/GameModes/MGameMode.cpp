// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/MGameMode.h"
#include "EngineUtils.h"
#include "AI/MAIController.h"
#include "Framework/MAssetManager.h"
#include "Framework/CapturePoint.h"
#include "GameFramework/PlayerStart.h"
#include "Character/MPlayerCharacter.h"
#include "Controllers/MPlayerController.h"
#include "Player/MPlayerState.h"
#include "Network/MGameSession.h"
#include "BrainComponent.h"
#include "AI/MinionBarrack.h"

AMGameMode::AMGameMode()
{
	GameSessionClass = AMGameSession::StaticClass();
}

APlayerController* AMGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController=Super::SpawnPlayerController(InRemoteRole, Options);
	IGenericTeamAgentInterface* NewPlayerTeamInterface=Cast<IGenericTeamAgentInterface>(NewPlayerController);
	FGenericTeamId TeamID=GetTeamIDForPlayer(NewPlayerController);
	UE_LOG(LogTemp, Warning, TEXT("TeamID : %d"), TeamID.GetId());
	
	if (NewPlayerTeamInterface)
		NewPlayerTeamInterface->SetGenericTeamId(TeamID);

	NewPlayerController->StartSpot=FindNextStartSpotForTeam(TeamID);
	return NewPlayerController;
	
}

void AMGameMode::StartPlay()
{
	Super::StartPlay();

	if (ACapturePoint* CP = GetCapturePoint())
		CP->OnCapturePointVictory.AddUObject(this, &AMGameMode::OnCapturePointVictory);
}


UClass* AMGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (AMPlayerState* MPS = InController->GetPlayerState<AMPlayerState>())
		if (TSubclassOf<APawn> SelectedClass = MPS->GetSelectedPawnClass())
			return SelectedClass;

	return BackupPawn;
}

APawn* AMGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayer);
	FGenericTeamId TeamId = GetTeamIDForPlayer(NewPlayer);

	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamId);
	}
	
	StartSpot=FindNextStartSpotForTeam(TeamId);
	NewPlayer->StartSpot=StartSpot;
	
	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

FGenericTeamId AMGameMode::GetTeamIDForPlayer(const AController* InController) const
{
	AMPlayerState* CPlayerState = InController->GetPlayerState<AMPlayerState>();
	if (CPlayerState && CPlayerState->GetSelectedPawnClass())
	{
		return CPlayerState->GetTeamIdBasedOnSlot();
	}
	
	static int PlayerCount=0;
	PlayerCount++;
	return FGenericTeamId(PlayerCount%2);
}

AActor* AMGameMode::FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const
{
	const FName* StartSpotTag=TeamStartSpotTagMap.Find(TeamID);
	if (!StartSpotTag) return nullptr;

	UWorld* World=GetWorld();

	APlayerStart* nullStart=nullptr;
	

	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		if (It->PlayerStartTag==*StartSpotTag)
		{
			It->PlayerStartTag=FName("Taken");
			return *It;
		}
		nullStart=*It;
	}
	
	return nullStart;
}


ACapturePoint* AMGameMode::GetCapturePoint() const
{
	for (TActorIterator<ACapturePoint> It(GetWorld()); It; ++It)
		return *It;
	return nullptr;
}


void AMGameMode::OnCapturePointVictory(int32 WinningTeamId)
{
	ACapturePoint* CP = GetCapturePoint();
	MatchFinished(CP, WinningTeamId);
}

void AMGameMode::MatchFinished(AActor* ViewTarget, int32 WinningTeamId)
{
	for (TActorIterator<AMAIController> It(GetWorld()); It; ++It)
	{
		if (It->GetBrainComponent())
			It->GetBrainComponent()->StopLogic("Match Ended");
	}

	for (TActorIterator<AMPlayerController> It(GetWorld()); It; ++It)
		It->MatchFinished(ViewTarget, WinningTeamId);
	
	for (TActorIterator<AMinionBarrack> It(GetWorld()); It; ++It)
		It->StopSpawning();
}
