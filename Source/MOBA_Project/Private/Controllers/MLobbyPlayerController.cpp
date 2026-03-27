// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/MLobbyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Framework/MGameInstance.h"
#include "Framework/MGameState.h"
#include "Widgets/MLobbyWidget.h"
#include "Widgets/Subsystems/WidgetManagerSubsystem.h"

AMLobbyPlayerController::AMLobbyPlayerController()
{
	bAutoManageActiveCameraTarget = false;
}

void AMLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UMGameInstance* GI = GetGameInstance<UMGameInstance>();
	if (!GI) return;

	const FString& Nickname = GI->GetPendingNickName();
	if (!Nickname.IsEmpty())
		Server_SetPlayerNickName(Nickname);
}

void AMLobbyPlayerController::Server_SetPlayerNickName_Implementation(const FString& NickName)
{
	if (APlayerState* PS = GetPlayerState<APlayerState>())
		PS->SetPlayerName(NickName);
}

bool AMLobbyPlayerController::Server_SetPlayerNickName_Validate(const FString& NickName)
{
	return !NickName.IsEmpty() && NickName.Len() <= 32;
}


void AMLobbyPlayerController::Server_RequestSlotChange_Implementation(uint8 NewSlotId)
{
	AMGameState* GS = GetWorld() ? GetWorld()->GetGameState<AMGameState>() : nullptr;
	if (!GS) return;
	
	GS->RequestPlayerSelectionChange(GetPlayerState<APlayerState>(), NewSlotId);
}

bool AMLobbyPlayerController::Server_RequestSlotChange_Validate(uint8 NewSlotId)
{
	return true;
}


void AMLobbyPlayerController::Server_StartHeroSelection_Implementation()
{
	if (!HasAuthority() || !GetWorld()) return;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AMLobbyPlayerController* PC = Cast<AMLobbyPlayerController>(*It))
			PC->Client_StartHeroSelection();
	}
}

bool AMLobbyPlayerController::Server_StartHeroSelection_Validate()
{
	return true;
}

void AMLobbyPlayerController::Client_StartHeroSelection_Implementation()
{
	OnSwitchToHeroSelection.ExecuteIfBound();
}


void AMLobbyPlayerController::Server_RequestStartMatch_Implementation()
{
	if (UMGameInstance* GI = GetGameInstance<UMGameInstance>())
		GI->StartMatch();
}

bool AMLobbyPlayerController::Server_RequestStartMatch_Validate()
{
	return true;
}
