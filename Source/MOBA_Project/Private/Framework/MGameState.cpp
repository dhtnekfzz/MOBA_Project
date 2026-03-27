// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/MGameState.h"
#include "DataAssets/StartUpData/DataAsset_PlayerStartUpData.h"
#include "Net/UnrealNetwork.h"

void AMGameState::RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot)
{
	if (!HasAuthority() || IsSlotOccupied(DesiredSlot)) return;

	FPlayerSelection* PlayerSelectionPtr = PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.IsForPlayer(RequestingPlayer);
		}
	);

	if (PlayerSelectionPtr)
	{
		PlayerSelectionPtr->SetSlot(DesiredSlot);
	}
	else
	{
		PlayerSelectionArray.Add(FPlayerSelection(DesiredSlot, RequestingPlayer));
	}

	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}

void AMGameState::SetCharacterSelected(const APlayerState* SelectingPlayer, const UDA_MCharacterDefinition* SelectedDefinition)
{
	if (IsDefinitionSelected(SelectedDefinition)) return;

	FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.IsForPlayer(SelectingPlayer);
		}
	);

	if (FoundPlayerSelection)
	{
		FoundPlayerSelection->SetCharacterDefinition(SelectedDefinition);
		OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
	}
}

bool AMGameState::IsSlotOccupied(uint8 SlotId) const
{
	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray)
	{
		if (PlayerSelection.GetPlayerSlot() == SlotId)
		{
			return true;
		}
	}

	return false;
}


bool AMGameState::IsDefinitionSelected(const UDA_MCharacterDefinition* Definition) const
{
	const FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.GetCharacterDefinition() == Definition;
		}
	);

	return FoundPlayerSelection != nullptr;
}

void AMGameState::SetCharacterDeselected(const UDA_MCharacterDefinition* DefinitionToDeselect)
{
	if (!DefinitionToDeselect) return;

	FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.GetCharacterDefinition() == DefinitionToDeselect;
		}
	);

	if (FoundPlayerSelection)
	{
		FoundPlayerSelection->SetCharacterDefinition(nullptr);
		OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
	}
}

void AMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AMGameState, PlayerSelectionArray, COND_None, REPNOTIFY_Always);
}

const TArray<FPlayerSelection>& AMGameState::GetPlayerSelection() const
{
	return PlayerSelectionArray;
}

bool AMGameState::CanStartHeroSelection() const
{
	return PlayerSelectionArray.Num() == PlayerArray.Num();
}

bool AMGameState::CanStartMatch() const
{
	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray)
	{
		if (PlayerSelection.GetCharacterDefinition() == nullptr)
		{
			return false;
		}
	}

	return true;
}

void AMGameState::OnRep_PlayerSelectionArray()
{
	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}
