// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MTypes/MStructTypes.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

class UDataAsset_ShopItem;
class UMAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilityCanCastUpdated, bool /*bCanCast*/);
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	FOnAbilityCanCastUpdated OnAbilityCanCastUpdated;
	
	UInventoryItem();

	void InitItem(const FMInventoryItemHandle& NewHandle, const UDataAsset_ShopItem* NewShopItem, UMAbilitySystemComponent* InASC);

	void SetGrantedAbilitySpecHandle(FGameplayAbilitySpecHandle InHandle) { GrantedAbilitySpecHandle = InHandle; }
	
	bool IsValid() const { return ShopItem != nullptr; }
	bool IsStackFull() const;
	bool AddStackCount();
	bool ReduceStackCount();           
	bool SetStackCount(int32 NewCount);
	bool IsForItem(const UDataAsset_ShopItem* Item) const;
	bool IsGrantingAbility(TSubclassOf<UMGameplayAbility> Class) const;
	bool IsGrantingAnyAbility() const;

	FORCEINLINE const UDataAsset_ShopItem* GetShopItem() const { return ShopItem; }
	FORCEINLINE FMInventoryItemHandle GetHandle() const { return Handle; }
	FORCEINLINE int32 GetStackCount() const { return StackCount; }
	FORCEINLINE int32 GetItemSlot() const { return Slot; }
	FORCEINLINE FGameplayAbilitySpecHandle GetGrantedAbilitySpecHandle() const { return GrantedAbilitySpecHandle; }

	void SetSlot(int32 NewSlot) { Slot = NewSlot; }

	bool TryActivateGrantedAbility();
	void ApplyConsumeEffect();
	void RemoveGASModifications();

	float GetAbilityCooldownTimeRemaining() const;
	float GetAbilityCooldownDuration() const;
	float GetAbilityManaCost() const;
	bool  CanCastAbility() const;

private:
	void ApplyGASModifications();
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);

	UPROPERTY()
	TObjectPtr<const UDataAsset_ShopItem> ShopItem  = nullptr;
	
	UPROPERTY()
	TObjectPtr<UMAbilitySystemComponent> CachedASC = nullptr;

	FMInventoryItemHandle Handle;
	FActiveGameplayEffectHandle AppliedEquippedEffectHandle;
	FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;

	int32 StackCount = 1;
	int32 Slot = 0;
	
};
