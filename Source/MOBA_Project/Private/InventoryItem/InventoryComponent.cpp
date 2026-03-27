// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryItem/InventoryComponent.h"
#include "InventoryItem/InventoryItem.h"
#include "Framework/MAssetManager.h"
#include "GAS/MAbilitySystemComponent.h"
#include "GAS/MAttributeSet.h"
#include "GAS/MHeroAttributeSet.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UInventoryComponent::InitWithASC(UMAbilitySystemComponent* InASC)
{
	CachedASC = InASC;
	if (!CachedASC) return;

	CachedASC->AbilityCommittedCallbacks.AddUObject(this, &UInventoryComponent::AbilityCommitted);
}

void UInventoryComponent::TryPurchase(const UDataAsset_ShopItem* Item)
{
	if (!CachedASC) return;
	Server_Purchase(Item);
}

void UInventoryComponent::TryActivateItem(const FMInventoryItemHandle& Handle)
{
	if (GetInventoryItemByHandle(Handle))
		Server_ActivateItem(Handle);
}

void UInventoryComponent::SellItem(const FMInventoryItemHandle& Handle)
{
	Server_SellItem(Handle);
}

void UInventoryComponent::ItemSlotChanged(const FMInventoryItemHandle& Handle, int32 NewSlot)
{
	if (UInventoryItem* Item = GetInventoryItemByHandle(Handle))
		Item->SetSlot(NewSlot);
}

void UInventoryComponent::TryActivateItemInSlot(int32 SlotNumber)
{
	for (TPair<FMInventoryItemHandle, UInventoryItem*>& Pair : InventoryItemMap)
	{
		if (Pair.Value && Pair.Value->GetItemSlot() == SlotNumber)
		{
			Server_ActivateItem(Pair.Key);
			return;
		}
	}
}


float UInventoryComponent::GetGold() const
{
	if (!CachedASC) return 0.f;
	bool bFound = false;
	const float Gold = CachedASC->GetGameplayAttributeValue(UMHeroAttributeSet::GetGoldAttribute(), bFound);
	return bFound ? Gold : 0.f;
}

UInventoryItem* UInventoryComponent::GetInventoryItemByHandle(const FMInventoryItemHandle& Handle) const
{
	UInventoryItem* const* Found = InventoryItemMap.Find(Handle);
	return Found ? *Found : nullptr;
}

// Crunch: IsAllSlotOccupied()
bool UInventoryComponent::IsAllSlotOccupied() const
{
	return InventoryItemMap.Num() >= Capacity;
}

bool UInventoryComponent::IsFullFor(const UDataAsset_ShopItem* Item) const
{
	if (!Item) return false;
	// 슬롯이 남아있으면 무조건 추가 가능
	if (!IsAllSlotOccupied()) return false;
	// 슬롯이 가득 찬 경우에만 스태킹 가능 여부 체크
	return GetAvailableStackForItem(Item) == nullptr;
}

UInventoryItem* UInventoryComponent::GetAvailableStackForItem(const UDataAsset_ShopItem* Item) const
{
	if (!Item || !Item->IsStackable()) return nullptr;

	for (const TPair<FMInventoryItemHandle, UInventoryItem*>& Pair : InventoryItemMap)
	{
		if (Pair.Value && Pair.Value->IsForItem(Item) && !Pair.Value->IsStackFull())
			return Pair.Value;
	}
	return nullptr;
}

UInventoryItem* UInventoryComponent::TryGetItemForShopItem(const UDataAsset_ShopItem* Item) const
{
	if (!Item) return nullptr;

	for (const TPair<FMInventoryItemHandle, UInventoryItem*>& Pair : InventoryItemMap)
	{
		if (Pair.Value && Pair.Value->GetShopItem() == Item)
			return Pair.Value;
	}
	return nullptr;
}

bool UInventoryComponent::FindIngredientForItem(const UDataAsset_ShopItem* CombinationItem, TArray<UInventoryItem*>& OutIngredients, const TArray<const UDataAsset_ShopItem*>& IngredientToIgnore)
{
	if (!CombinationItem) return false;

	const TArray<TSoftObjectPtr<UDataAsset_ShopItem>>& SoftIngredients =
		CombinationItem->GetIngredientItems();

	if (SoftIngredients.IsEmpty()) return false;

	for (const TSoftObjectPtr<UDataAsset_ShopItem>& Soft : SoftIngredients)
	{
		const UDataAsset_ShopItem* Ingredient = Soft.Get();
		if (!Ingredient)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[InventoryComponent] FindIngredientForItem: Ingredient not loaded for '%s'. "
				     "Make sure all ShopItems are registered as PrimaryAssets."),
				*CombinationItem->GetItemName().ToString());
			return false;
		}

		// 방금 구입한 아이템은 인벤토리 조회 없이 통과
		if (IngredientToIgnore.Contains(Ingredient)) continue;

		UInventoryItem* FoundItem = TryGetItemForShopItem(Ingredient);
		if (!FoundItem) return false;

		OutIngredients.Add(FoundItem);
	}

	return true;
}


void UInventoryComponent::Server_Purchase_Implementation(const UDataAsset_ShopItem* Item)
{
	if (!Item || !CachedASC) return;
	if (Item->GetPrice() > GetGold()) return;

	if (!IsFullFor(Item))
	{
		CachedASC->ApplyModToAttribute(UMHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -Item->GetPrice());
		GrantItem(Item);
		return;
	}

	if (TryItemCombination(Item))
	{
		CachedASC->ApplyModToAttribute(UMHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -Item->GetPrice());
	}
}

bool UInventoryComponent::Server_Purchase_Validate(const UDataAsset_ShopItem* Item)
{
	return true;
}

void UInventoryComponent::Server_ActivateItem_Implementation(FMInventoryItemHandle Handle)
{
	UInventoryItem* InventoryItem = GetInventoryItemByHandle(Handle);
	if (!InventoryItem) return;

	InventoryItem->TryActivateGrantedAbility();

	if (const UDataAsset_ShopItem* ShopItem = InventoryItem->GetShopItem())
	{
		if (ShopItem->IsConsumable())
			ConsumeItem(InventoryItem);
	}
}

bool UInventoryComponent::Server_ActivateItem_Validate(FMInventoryItemHandle Handle)
{
	return true;
}

void UInventoryComponent::Server_SellItem_Implementation(FMInventoryItemHandle Handle)
{
	UInventoryItem* Item = GetInventoryItemByHandle(Handle);
	if (!Item || !Item->IsValid()) return;

	const float SellPrice = Item->GetShopItem()->GetSellPrice();
	CachedASC->ApplyModToAttribute(
		UMHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive,
		SellPrice * Item->GetStackCount());

	RemoveItem(Item);
}

bool UInventoryComponent::Server_SellItem_Validate(FMInventoryItemHandle Handle)
{
	return true;
}

void UInventoryComponent::Client_ItemAdded_Implementation(FMInventoryItemHandle Handle,
	const UDataAsset_ShopItem* Item, FGameplayAbilitySpecHandle GrantedSpecHandle, int32 Slot)
{
	if (GetOwner()->HasAuthority()) return;

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->InitItem(Handle, Item, CachedASC);
	InventoryItem->SetGrantedAbilitySpecHandle(GrantedSpecHandle);
	InventoryItemMap.Add(Handle, InventoryItem);
	HandleToSlotMap.Add(Handle, Slot);
	OccupiedSlots.Add(Slot);

	OnItemAdded.Broadcast(InventoryItem, Slot);
}

void UInventoryComponent::Client_ItemRemoved_Implementation(FMInventoryItemHandle Handle)
{
	if (GetOwner()->HasAuthority()) return;

	UInventoryItem* InventoryItem = GetInventoryItemByHandle(Handle);
	if (!InventoryItem) return;

	InventoryItem->RemoveGASModifications();

	if (const int32* Slot = HandleToSlotMap.Find(Handle))
		OccupiedSlots.Remove(*Slot);

	InventoryItemMap.Remove(Handle);
	HandleToSlotMap.Remove(Handle);
	OnItemRemoved.Broadcast(Handle);
}

void UInventoryComponent::Client_ItemStackCountChanged_Implementation(FMInventoryItemHandle Handle, int32 NewCount)
{
	if (GetOwner()->HasAuthority()) return;

	UInventoryItem* Item = GetInventoryItemByHandle(Handle);
	if (!Item) return;

	Item->SetStackCount(NewCount);
	OnItemStackCountChanged.Broadcast(Handle, NewCount);
}


void UInventoryComponent::GrantItem(const UDataAsset_ShopItem* NewItem)
{
	if (!GetOwner()->HasAuthority()) return;
	
	if (UInventoryItem* StackItem = GetAvailableStackForItem(NewItem))
	{
		StackItem->AddStackCount();
		OnItemStackCountChanged.Broadcast(StackItem->GetHandle(), StackItem->GetStackCount());
		Client_ItemStackCountChanged(StackItem->GetHandle(), StackItem->GetStackCount());
		return;
	}

	if (TryItemCombination(NewItem)) return;

	// 3. 새 슬롯에 추가
	const int32 NewSlot = GetNextFreeSlot();
	const FMInventoryItemHandle NewHandle = FMInventoryItemHandle::CreateHandle();

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->InitItem(NewHandle, NewItem, CachedASC);
	InventoryItemMap.Add(NewHandle, InventoryItem);
	HandleToSlotMap.Add(NewHandle, NewSlot);
	OccupiedSlots.Add(NewSlot);

	// 서버에서도 브로드캐스트 (ShopSubsystem 은 클라 전용이라 서버에선 무시됨)
	OnItemAdded.Broadcast(InventoryItem, NewSlot);
	Client_ItemAdded(NewHandle, NewItem, InventoryItem->GetGrantedAbilitySpecHandle(), NewSlot);
}

void UInventoryComponent::ConsumeItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority() || !Item) return;

	Item->ApplyConsumeEffect();

	if (!Item->ReduceStackCount())
	{
		RemoveItem(Item);
	}
	else
	{
		OnItemStackCountChanged.Broadcast(Item->GetHandle(), Item->GetStackCount());
		Client_ItemStackCountChanged(Item->GetHandle(), Item->GetStackCount());
	}
}

void UInventoryComponent::RemoveItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority() || !Item) return;

	Item->RemoveGASModifications();

	const FMInventoryItemHandle Handle = Item->GetHandle();

	if (const int32* Slot = HandleToSlotMap.Find(Handle))
		OccupiedSlots.Remove(*Slot);

	OnItemRemoved.Broadcast(Handle);
	InventoryItemMap.Remove(Handle);
	HandleToSlotMap.Remove(Handle);
	Client_ItemRemoved(Handle);
}


bool UInventoryComponent::TryItemCombination(const UDataAsset_ShopItem* NewItem)
{
	if (!GetOwner()->HasAuthority()) return false;

	UMAssetManager& AssetManager = UMAssetManager::Get();

	const FItemCollection* CombinationItems = UMAssetManager::Get().GetCombinationForItem(NewItem);
	if (!CombinationItems)
		return false;

	for (const UDataAsset_ShopItem* CombinationItem : CombinationItems->GetItems())
	{
		TArray<UInventoryItem*> Ingredients;
		if (!FindIngredientForItem(CombinationItem, Ingredients, TArray<const UDataAsset_ShopItem*>{NewItem}))
			continue;

		for (UInventoryItem* Ingredient : Ingredients)
		{
			RemoveItem(Ingredient);
		}

		GrantItem(CombinationItem);
		return true;
	}

	return false;
}

int32 UInventoryComponent::GetNextFreeSlot() const
{
	for (int32 i = 0; i < Capacity; ++i)
	{
		if (!OccupiedSlots.Contains(i))
			return i;
	}

	return 0;
}

void UInventoryComponent::AbilityCommitted(UGameplayAbility* CommittedAbility)
{
	if (!CommittedAbility) return;

	float CooldownTimeRemaining = 0.f;
	float CooldownDuration      = 0.f;
	CommittedAbility->GetCooldownTimeRemainingAndDuration(
		CommittedAbility->GetCurrentAbilitySpecHandle(),
		CommittedAbility->GetCurrentActorInfo(),
		CooldownTimeRemaining,
		CooldownDuration);

	for (TPair<FMInventoryItemHandle, UInventoryItem*>& Pair : InventoryItemMap)
	{
		if (!Pair.Value) continue;
		
		if (Pair.Value->IsGrantingAbility(CommittedAbility->GetClass()))
		{
			OnItemAbilityCommitted.Broadcast(Pair.Key, CooldownDuration, CooldownTimeRemaining);
		}
	}
}
