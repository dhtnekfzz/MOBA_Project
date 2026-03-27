// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MVVM/MVVM_InventorySlot.h"
#include "InventoryItem/InventoryItem.h"
#include "DataAssets/DataAsset_ShopItem.h"
#include "GAS/MAbilitySystemStatics.h"


void UMVVM_InventorySlot::BindToItem(UInventoryItem* Item)
{
	UnbindAndClear();

	if (!Item || !Item->IsValid()) return;

	CachedItem = Item;
	SetbIsEmpty(false);
	SetStackCount(Item->GetStackCount());
	SetManaCost(Item->GetAbilityManaCost());
	SetbCanCast(Item->CanCastAbility());
	SetIcon(Item->GetShopItem()->GetIcon());

	const float CooldownLeft = Item->GetAbilityCooldownTimeRemaining();
	const float CooldownDur  = Item->GetAbilityCooldownDuration();

	SetCooldownDuration(CooldownDur);
	SetCooldownRemaining(CooldownLeft);

	if (CooldownLeft > 0.f)
	{
		StartCooldownTick();
	}

	CachedItem->OnAbilityCanCastUpdated.AddUObject(this, &UMVVM_InventorySlot::OnCanCastUpdated);
}


void UMVVM_InventorySlot::UnbindAndClear()
{
	if (CachedItem)
	{
		CachedItem->OnAbilityCanCastUpdated.RemoveAll(this);
		CachedItem = nullptr;
	}

	FinishCooldown();

	SetbIsEmpty(true);
	SetIcon(nullptr);
	SetStackCount(0);
	SetCooldownRemaining(0.f);
	SetCooldownDuration(0.f);
	SetManaCost(0.f);
	SetbCanCast(false);
}

void UMVVM_InventorySlot::NotifyCooldownStarted(float Duration, float TimeRemaining)
{
	SetCooldownDuration(Duration);
	SetCooldownRemaining(TimeRemaining);
	StartCooldownTick();
}


void UMVVM_InventorySlot::OnCanCastUpdated(bool bNew)
{
	SetbCanCast(bNew);
}

void UMVVM_InventorySlot::StartCooldownTick()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(CooldownTickTimer);
	World->GetTimerManager().SetTimer(
		CooldownTickTimer,
		this,
		&UMVVM_InventorySlot::TickCooldown,
		TickInterval,
		/*bLoop=*/true);
}

void UMVVM_InventorySlot::TickCooldown()
{
	const float NewRemaining = CooldownRemaining - TickInterval;

	if (NewRemaining <= 0.f)
	{
		FinishCooldown();
		return;
	}

	SetCooldownRemaining(NewRemaining);
}

void UMVVM_InventorySlot::FinishCooldown()
{
	if (UWorld* World = GetWorld())
		World->GetTimerManager().ClearTimer(CooldownTickTimer);

	SetCooldownRemaining(0.f);
}


void UMVVM_InventorySlot::SetIcon(UTexture2D* NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(Icon, NewValue);
}

void UMVVM_InventorySlot::SetStackCount(int32 NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(StackCount, NewValue);
}

void UMVVM_InventorySlot::SetCooldownRemaining(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(CooldownRemaining, NewValue);
}

void UMVVM_InventorySlot::SetCooldownDuration(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(CooldownDuration, NewValue);
}

void UMVVM_InventorySlot::SetManaCost(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(ManaCost, NewValue);
}

void UMVVM_InventorySlot::SetbCanCast(bool bNewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(bCanCast, bNewValue);
}

void UMVVM_InventorySlot::SetbIsEmpty(bool bNewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(bIsEmpty, bNewValue);
}
