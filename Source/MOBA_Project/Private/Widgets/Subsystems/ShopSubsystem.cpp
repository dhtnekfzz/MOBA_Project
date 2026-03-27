// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Subsystems/ShopSubsystem.h"
#include "Framework/MAssetManager.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "DataAssets/DataAsset_ShopItem.h"
#include "GAS/MAbilitySystemComponent.h"
#include "GAS/MHeroAttributeSet.h"
#include "InventoryItem/InventoryComponent.h"
#include "InventoryItem/InventoryItem.h"
#include "Widgets/MVVM/MVVM_InventorySlot.h"
#include "Widgets/MVVM/MVVM_ShopItem.h"


void UShopSubsystem::UpdateHandleToVMMapping(const FMInventoryItemHandle& Handle, UMVVM_InventorySlot* VM)
{
	if (!Handle.IsValid()) return;

	if (VM)
		HandleToVMMap.Add(Handle, VM);
	else
		HandleToVMMap.Remove(Handle);
	
}

void UShopSubsystem::RequestShopItemsLoad()
{
	if (bIconsLoaded)
	{
		OnIconsLoaded();
		return;
	}

	UMAssetManager::Get().LoadShopItems(FStreamableDelegate::CreateUObject(this, &UShopSubsystem::OnIconsLoaded));
}


void UShopSubsystem::BindToInventoryComponent(UInventoryComponent* InvComp, int32 Capacity)
{
	UnbindFromInventoryComponent();
	if (!InvComp) return;

	CachedInvComp = InvComp;

	SlotViewModels.Empty();
	SlotViewModels.Reserve(Capacity);
	for (int32 i = 0; i < Capacity; ++i)
		SlotViewModels.Add(NewObject<UMVVM_InventorySlot>(this));

	HandleToVMMap.Empty();

	InvComp->OnItemAdded.AddUObject(this, &UShopSubsystem::OnItemAdded);
	InvComp->OnItemRemoved.AddUObject(this, &UShopSubsystem::OnItemRemoved);
	InvComp->OnItemStackCountChanged.AddUObject(this, &UShopSubsystem::OnStackCountChanged);
	InvComp->OnItemAbilityCommitted.AddUObject(this, &UShopSubsystem::OnItemAbilityCommitted);
}

void UShopSubsystem::UnbindFromInventoryComponent()
{
	if (!CachedInvComp) return;

	CachedInvComp->OnItemAdded.RemoveAll(this);
	CachedInvComp->OnItemRemoved.RemoveAll(this);
	CachedInvComp->OnItemStackCountChanged.RemoveAll(this);
	CachedInvComp->OnItemAbilityCommitted.RemoveAll(this);

	for (UMVVM_InventorySlot* Slot : SlotViewModels)
		if (Slot) Slot->UnbindAndClear();

	SlotViewModels.Empty();
	HandleToVMMap.Empty();
	CachedInvComp = nullptr;
}


void UShopSubsystem::BindToASC(UMAbilitySystemComponent* InASC)
{
	UnbindFromASC();
	if (!InASC) return;

	CachedASC = InASC;

	InASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetGoldAttribute()).AddUObject(this, &UShopSubsystem::OnGoldChanged);

	bool bFound = false;
	const float CurrentGold = InASC->GetGameplayAttributeValue(UMHeroAttributeSet::GetGoldAttribute(), bFound);
	if (bFound)
		for (TObjectPtr<UMVVM_ShopItem>& VM : ShopItemViewModels)
			if (VM) VM->UpdateAffordability(CurrentGold);
}

void UShopSubsystem::UnbindFromASC()
{
	if (!CachedASC) return;
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetGoldAttribute()).RemoveAll(this);
	CachedASC = nullptr;
}


UMVVM_ShopItem* UShopSubsystem::GetViewModelForItem(const UDataAsset_ShopItem* Item) const
{
	if (!Item) return nullptr;
	if (const TObjectPtr<UMVVM_ShopItem>* Found = DataAssetToVMMap.Find(Item))
		return Found->Get();
	return nullptr;
}

void UShopSubsystem::OnItemAdded(UInventoryItem* Item, int32 Slot)
{
	if (!Item || !SlotViewModels.IsValidIndex(Slot)) return;

	UMVVM_InventorySlot* SlotVM = SlotViewModels[Slot];
	if (!SlotVM) return;

	SlotVM->BindToItem(Item);
	HandleToVMMap.Add(Item->GetHandle(), SlotVM);
	OnInventorySlotItemSet.Broadcast(Item->GetHandle(), Slot);
}

void UShopSubsystem::OnItemRemoved(const FMInventoryItemHandle& Handle)
{
	TObjectPtr<UMVVM_InventorySlot>* FoundVM = HandleToVMMap.Find(Handle);
	if (!FoundVM || !(*FoundVM)) return;

	(*FoundVM)->UnbindAndClear();
	HandleToVMMap.Remove(Handle);
	OnInventorySlotItemCleared.Broadcast(Handle);
}

void UShopSubsystem::OnStackCountChanged(const FMInventoryItemHandle& Handle, int32 NewCount)
{
	if (TObjectPtr<UMVVM_InventorySlot>* FoundVM = HandleToVMMap.Find(Handle))
		if (*FoundVM) (*FoundVM)->SetStackCount(NewCount);
}

void UShopSubsystem::OnItemAbilityCommitted(const FMInventoryItemHandle& Handle, float CooldownDuration, float CooldownRemaining)
{
	if (TObjectPtr<UMVVM_InventorySlot>* FoundVM = HandleToVMMap.Find(Handle))
		if (*FoundVM) (*FoundVM)->NotifyCooldownStarted(CooldownDuration, CooldownRemaining);
}


void UShopSubsystem::OnGoldChanged(const FOnAttributeChangeData& ChangeData)
{
	const float NewGold = ChangeData.NewValue;
	for (TObjectPtr<UMVVM_ShopItem>& VM : ShopItemViewModels)
		if (VM) VM->UpdateAffordability(NewGold);
}

void UShopSubsystem::OnIconsLoaded()
{
	InitShopItemViewModels();
	bIconsLoaded = true;
}

void UShopSubsystem::InitShopItemViewModels()
{
	ShopItemViewModels.Empty();
	DataAssetToVMMap.Empty();

	TArray<const UDataAsset_ShopItem*> LoadedItems;
	UMAssetManager::Get().GetLoadedShopItems(LoadedItems);

	ShopItemViewModels.Reserve(LoadedItems.Num());
	DataAssetToVMMap.Reserve(LoadedItems.Num());

	for (const UDataAsset_ShopItem* Item : LoadedItems)
	{
		if (!Item) continue;

		UTexture2D* LoadedIcon = Item->GetIcon();

		UMVVM_ShopItem* VM = NewObject<UMVVM_ShopItem>(this);
		VM->InitFromDataAsset(Item, LoadedIcon);
		ShopItemViewModels.Add(VM);
		DataAssetToVMMap.Add(Item, VM);
	}

	if (CachedASC)
	{
		bool bFound = false;
		const float CurrentGold = CachedASC->GetGameplayAttributeValue(UMHeroAttributeSet::GetGoldAttribute(), bFound);
		
		if (bFound)
			for (TObjectPtr<UMVVM_ShopItem>& VM : ShopItemViewModels)
				if (VM) VM->UpdateAffordability(CurrentGold);
	}
}
