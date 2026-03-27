// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "MTypes/MStructTypes.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "ShopSubsystem.generated.h"

class UInventoryItem;
class UInventoryComponent;
class UMVVM_InventorySlot;
class UMVVM_ShopItem;
class UDataAsset_ShopItem;
class UMAbilitySystemComponent;
struct FOnAttributeChangeData;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventorySlotItemSet, const FMInventoryItemHandle&, int32 );
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventorySlotItemCleared, const FMInventoryItemHandle&);


UCLASS()
class MOBA_PROJECT_API UShopSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	FOnInventorySlotItemSet OnInventorySlotItemSet;
	FOnInventorySlotItemCleared OnInventorySlotItemCleared;

	void UpdateHandleToVMMapping(const FMInventoryItemHandle& Handle, UMVVM_InventorySlot* VM);
	
	void RequestShopItemsLoad();
	
	void BindToInventoryComponent(UInventoryComponent* InvComp, int32 Capacity);
	void UnbindFromInventoryComponent();
	
	void BindToASC(UMAbilitySystemComponent* InASC);
	void UnbindFromASC();

	FORCEINLINE const TArray<TObjectPtr<UMVVM_ShopItem>>& GetShopItemViewModels() const
	{
		return ShopItemViewModels;
	}

	FORCEINLINE const TArray<TObjectPtr<UMVVM_InventorySlot>>& GetSlotViewModels() const
	{
		return SlotViewModels;
	}

	UMVVM_ShopItem* GetViewModelForItem(const UDataAsset_ShopItem* Item) const;

private:
	void OnIconsLoaded();

	void InitShopItemViewModels();

	void OnItemAdded(UInventoryItem* Item, int32 Slot);
	void OnItemRemoved(const FMInventoryItemHandle& Handle);
	void OnStackCountChanged(const FMInventoryItemHandle& Handle, int32 NewCount);
	void OnItemAbilityCommitted(const FMInventoryItemHandle& Handle,
	                            float CooldownDuration, float CooldownRemaining);

	void OnGoldChanged(const FOnAttributeChangeData& ChangeData);

	UPROPERTY()
	TArray<TObjectPtr<UMVVM_ShopItem>> ShopItemViewModels;

	UPROPERTY()
	TMap<const UDataAsset_ShopItem*, TObjectPtr<UMVVM_ShopItem>> DataAssetToVMMap;

	UPROPERTY()
	TArray<TObjectPtr<UMVVM_InventorySlot>> SlotViewModels;

	UPROPERTY()
	TMap<FMInventoryItemHandle, TObjectPtr<UMVVM_InventorySlot>> HandleToVMMap;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> CachedInvComp;

	UPROPERTY()
	TObjectPtr<UMAbilitySystemComponent> CachedASC;
	
	TSharedPtr<FStreamableHandle> IconLoadHandle;

	bool bIconsLoaded = false;
};
