// Fill out your copyright notice in the Description page of Project Settings.


#include "MTypes/MStructTypes.h"

#include "GameFramework/PlayerState.h"
#include "Slate/SGameLayerManager.h"

FMHeroBaseStats::FMHeroBaseStats()
	:Class {nullptr},
	Strength{0.f},
	Intelligence{0.f},
	StrengthGrowthRate{0.f},
	IntelligenceGrowthRate{0.f},
	BaseMaxHealth{0.f},
	BaseMaxMana(0.f),
	BaseAttackDamage{0.f},
	BaseArmor{0.f},
	BaseMoveSpeed{0.f}
{
	
}

FMInventoryItemHandle::FMInventoryItemHandle() : HandleId(GetInvalidId())
{
}

FMInventoryItemHandle::FMInventoryItemHandle(uint32 InHandleId) : HandleId(InHandleId)
{
}

FMInventoryItemHandle FMInventoryItemHandle::InvalidHandle()
{
	static FMInventoryItemHandle Invalid;
	return Invalid;
}

FMInventoryItemHandle FMInventoryItemHandle::CreateHandle()
{
	return FMInventoryItemHandle(GenerateNextId());
}

bool FMInventoryItemHandle::IsValid() const
{
	return HandleId != GetInvalidId();
}



uint32 FMInventoryItemHandle::GenerateNextId()
{
	static uint32 Counter=1;
	return Counter++;
}

bool operator==(const FMInventoryItemHandle& A, const FMInventoryItemHandle& B)
{
	return A.GetHandleId() == B.GetHandleId();
}

uint32 GetTypeHash(const FMInventoryItemHandle& Key)
{
	return Key.GetHandleId();
}

FItemCollection::FItemCollection() : Items()
{
}

FItemCollection::FItemCollection(const TArray<const UDataAsset_ShopItem*>& InItems) : Items{InItems}
{
}

void FItemCollection::AddItem(const UDataAsset_ShopItem* NewItem, bool bAddUnique)
{
	if (bAddUnique && Contains(NewItem)) return;
	
	Items.Add(NewItem);
}

bool FItemCollection::Contains(const UDataAsset_ShopItem* Item) const
{
	return Items.Contains(Item);
}

const TArray<const UDataAsset_ShopItem*>& FItemCollection::GetItems() const
{
	return Items;
}

FPlayerSelection::FPlayerSelection(): Slot{GetInvalidSlot()}, PlayerUniqueId{FUniqueNetIdRepl::Invalid()}, PlayerNickName{}, CharacterDefinition{nullptr}
{
}

FPlayerSelection::FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState) : Slot(InSlot), CharacterDefinition(nullptr)
{
	if (InPlayerState)
	{
		PlayerUniqueId=InPlayerState->GetUniqueId();
		PlayerNickName=InPlayerState->GetPlayerName();
	}
}

bool FPlayerSelection::IsForPlayer(const APlayerState* PlayerState) const
{
	if (!PlayerState) return false;

#if WITH_EDITOR
	return PlayerState->GetPlayerName() == PlayerNickName;
#else
	return PlayerState->GetUniqueId() == GetPLayerUniqueId();
#endif
	
}

bool FPlayerSelection::IsValid() const
{
	
#if WITH_EDITOR
	return true;

#else
	if (!PlayerUniqueId.IsValid())
		return false;

	if (Slot == GetInvalidSlot())
		return false;

	if (Slot >= UCNetStatics::GetPlayerCountPerTeam() * 2)
		return false;

	return true;
#endif

}

uint8 FPlayerSelection::GetInvalidSlot()
{
	return uint8(235);
}
