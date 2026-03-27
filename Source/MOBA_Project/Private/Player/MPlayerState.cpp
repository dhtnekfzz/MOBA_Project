// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/MPlayerState.h"
#include "DataAssets/DA_MCharacterDefinition.h"
#include "Framework/MGameState.h"
#include "Net/UnrealNetwork.h"
#include "Network/MNetStatics.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MPlayerCharacter.h"
#include "Framework/MGameInstance.h"

AMPlayerState::AMPlayerState()
{
	bReplicates = true;
	SetNetUpdateFrequency(100.f);

	UMGameInstance* GI = Cast<UMGameInstance>(GetGameInstance());
	if (GI)
		GI->OnLoginCompleted.AddUObject(this, &ThisClass::SetPlayerNickname);
}

void AMPlayerState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMPlayerState, PlayerSelection);
}

void AMPlayerState::BeginPlay()
{
	Super::BeginPlay();

	MGameState = Cast<AMGameState>(UGameplayStatics::GetGameState(this));
	if (MGameState)
		MGameState->OnPlayerSelectionUpdated.AddUObject(this, &AMPlayerState::OnPlayerSelectionUpdated);
}

void AMPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (AMPlayerState* NewPS = Cast<AMPlayerState>(PlayerState))
		NewPS->PlayerSelection = PlayerSelection;
}


TSubclassOf<APawn> AMPlayerState::GetSelectedPawnClass() const
{
	const UDA_MCharacterDefinition* Def = PlayerSelection.GetCharacterDefinition();
	if (!Def) return nullptr;
	return Def->LoadCharacterClass();
}


FGenericTeamId AMPlayerState::GetTeamIdBasedOnSlot() const
{
	return PlayerSelection.GetPlayerSlot() < UMNetStatics::GetPlayerCountPerTeam() ? FGenericTeamId{0} : FGenericTeamId{1};
}

void AMPlayerState::SetPlayerNickname(bool bSuccess, const FString& NickName, const FString& Msg)
{
	if (bSuccess)
		Server_SetPlayerNickName(NickName);
}

void AMPlayerState::Server_SetPlayerNickName_Implementation(const FString& NickName)
{
	SetPlayerName(NickName);
}


void AMPlayerState::Server_SetSelectedCharacterDefinition_Implementation( const UDA_MCharacterDefinition* NewDefinition)
{
	if (!MGameState || !NewDefinition) return;

	if (MGameState->IsDefinitionSelected(NewDefinition)) return;

	if (PlayerSelection.GetCharacterDefinition())
		MGameState->SetCharacterDeselected(PlayerSelection.GetCharacterDefinition());

	PlayerSelection.SetCharacterDefinition(NewDefinition);
	MGameState->SetCharacterSelected(this, NewDefinition);
}

bool AMPlayerState::Server_SetSelectedCharacterDefinition_Validate( const UDA_MCharacterDefinition* NewDefinition)
{
	return true;
}

void AMPlayerState::OnPlayerSelectionUpdated(const TArray<FPlayerSelection>& NewSelections)
{
	for (const FPlayerSelection& Sel : NewSelections)
		if (Sel.IsForPlayer(this))
			PlayerSelection = Sel;
}
