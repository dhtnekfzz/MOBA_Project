// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/DataAsset_ShopItem.h"
#include "MTypes/MStructTypes.h"
#include "InventoryComponent.generated.h"

class UInventoryItem;
class UDataAsset_ShopItem;
class UMAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemAddedDelegate, UInventoryItem* /*NewItem*/,int32 /*Slot*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const FMInventoryItemHandle& /*Handle*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemStackCountChangedDelegate, const FMInventoryItemHandle& /*Handle*/, int32 /*NewCount*/);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemAbilityCommitted, const FMInventoryItemHandle& /*Handle*/, float /*CooldownDuration*/, float /*CooldownTimeRemaining*/);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MOBA_PROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	FOnItemAddedDelegate OnItemAdded;
	FOnItemRemovedDelegate OnItemRemoved;
	FOnItemStackCountChangedDelegate OnItemStackCountChanged;
	FOnItemAbilityCommitted OnItemAbilityCommitted;
	
	void InitWithASC(UMAbilitySystemComponent* InASC);

	void TryPurchase(const UDataAsset_ShopItem* Item);
	void TryActivateItem(const FMInventoryItemHandle& Handle);
	void SellItem(const FMInventoryItemHandle& Handle);
	void ItemSlotChanged(const FMInventoryItemHandle& Handle, int32 NewSlot);
	
	void TryActivateItemInSlot(int32 SlotNumber);

	float GetGold() const;
	FORCEINLINE int32 GetCapacity() const { return Capacity; }

	UInventoryItem* GetInventoryItemByHandle(const FMInventoryItemHandle& Handle) const;
	
	bool IsAllSlotOccupied() const;
	bool IsFullFor(const UDataAsset_ShopItem* Item) const;

	UInventoryItem* GetAvailableStackForItem(const UDataAsset_ShopItem* Item) const;
	UInventoryItem* TryGetItemForShopItem(const UDataAsset_ShopItem* Item) const;
	
	bool FindIngredientForItem(const UDataAsset_ShopItem* CombinationItem, TArray<UInventoryItem*>& OutIngredients, const TArray<const UDataAsset_ShopItem*>& IngredientToIgnore=TArray<const UDataAsset_ShopItem*>{});

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	int32 Capacity = 6;

	UPROPERTY()
	TObjectPtr<UMAbilitySystemComponent> CachedASC;

	UPROPERTY()
	TMap<FMInventoryItemHandle, UInventoryItem*> InventoryItemMap;

	UPROPERTY()
	TMap<FMInventoryItemHandle, int32> HandleToSlotMap;

	TSet<int32> OccupiedSlots;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UDataAsset_ShopItem* Item);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ActivateItem(FMInventoryItemHandle Handle);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SellItem(FMInventoryItemHandle Handle);

	void GrantItem(const UDataAsset_ShopItem* NewItem);
	void ConsumeItem(UInventoryItem* Item);
	void RemoveItem(UInventoryItem* Item);
	bool TryItemCombination(const UDataAsset_ShopItem* NewItem);

	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FMInventoryItemHandle Handle, const UDataAsset_ShopItem* Item, FGameplayAbilitySpecHandle GrantedSpecHandle, int32 Slot);
	
	UFUNCTION(Client, Reliable)
	void Client_ItemRemoved(FMInventoryItemHandle Handle);
	
	UFUNCTION(Client, Reliable)
	void Client_ItemStackCountChanged(FMInventoryItemHandle Handle, int32 NewCount);
	
	int32 GetNextFreeSlot() const;
	void AbilityCommitted(UGameplayAbility* CommittedAbility);
};
