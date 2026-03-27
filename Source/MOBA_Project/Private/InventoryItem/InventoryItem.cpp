// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItem/InventoryItem.h"
#include "DataAssets/DataAsset_ShopItem.h"
#include "GAS/MAbilitySystemComponent.h"
#include "GAS/MAbilitySystemStatics.h"
#include "GAS/MAttributeSet.h"

UInventoryItem::UInventoryItem()
{
}

void UInventoryItem::InitItem(const FMInventoryItemHandle& NewHandle, const UDataAsset_ShopItem* NewShopItem,
	UMAbilitySystemComponent* InASC)
{
	Handle=NewHandle;
	ShopItem=NewShopItem;
	CachedASC=InASC;

	if (CachedASC)
		CachedASC->GetGameplayAttributeValueChangeDelegate((UMAttributeSet::GetManaAttribute())).AddUObject(this, &ThisClass::ManaUpdated);

	ApplyGASModifications();
	
}

bool UInventoryItem::IsStackFull() const
{
	if (!ShopItem || !ShopItem->IsStackable()) return true;

	return StackCount >= ShopItem->GetMaxStackCount();
}

bool UInventoryItem::AddStackCount()
{
	if (IsStackFull()) return false;
	
	StackCount++;
	return true;
}

bool UInventoryItem::ReduceStackCount()
{
	--StackCount;
	if (StackCount <= 0) return false;
	
	return true;
}

bool UInventoryItem::SetStackCount(int32 NewCount)
{
	if (!ShopItem) return false;
	
	if (ShopItem->IsStackable() && NewCount > ShopItem->GetMaxStackCount()) return false;
	
	StackCount = NewCount;
	return true;
}

bool UInventoryItem::IsForItem(const UDataAsset_ShopItem* Item) const
{
	return ShopItem == Item;
}

bool UInventoryItem::IsGrantingAbility(TSubclassOf<UMGameplayAbility> Class) const
{
	if (!ShopItem || !ShopItem->GetGrantedAbility()) return false;
	
	return ShopItem->GetGrantedAbility() == Class;
}

bool UInventoryItem::IsGrantingAnyAbility() const
{
	return ShopItem && ShopItem->GetGrantedAbility() != nullptr;
}

bool UInventoryItem::TryActivateGrantedAbility()
{
	if (!CachedASC || !GrantedAbilitySpecHandle.IsValid()) return false;
	
	return CachedASC->TryActivateAbility(GrantedAbilitySpecHandle);
}

void UInventoryItem::ApplyConsumeEffect()
{
	if (!CachedASC || !ShopItem || !ShopItem->GetConsumedEffect()) return;

	FGameplayEffectContextHandle ContextHandle = CachedASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle = CachedASC->MakeOutgoingSpec(ShopItem->GetConsumedEffect(), 1.0f, ContextHandle);

	if (SpecHandle.IsValid())
		CachedASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UInventoryItem::RemoveGASModifications()
{
	if (!CachedASC) return;

	CachedASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetManaAttribute()).RemoveAll(this);

	if (AppliedEquippedEffectHandle.IsValid())
	{
		CachedASC->RemoveActiveGameplayEffect(AppliedEquippedEffectHandle);
		AppliedEquippedEffectHandle = FActiveGameplayEffectHandle();
	}

	if (GrantedAbilitySpecHandle.IsValid())
	{
		CachedASC->ClearAbility(GrantedAbilitySpecHandle);
		GrantedAbilitySpecHandle = FGameplayAbilitySpecHandle();
	}
}

float UInventoryItem::GetAbilityCooldownTimeRemaining() const
{
	if (!IsGrantingAnyAbility()) return 0.0f;
	
	return UMAbilitySystemStatics::GetCooldownRemainingFor(ShopItem->GetGrantedAbilityCDO(), *CachedASC);

}

float UInventoryItem::GetAbilityCooldownDuration() const
{
	if (!IsGrantingAnyAbility()) return 0.0f;
	
	return UMAbilitySystemStatics::GetCooldownDurationFor(ShopItem->GetGrantedAbilityCDO(), *CachedASC, 1);
}

float UInventoryItem::GetAbilityManaCost() const
{
	if (!ShopItem) return 0.0f;

	UMGameplayAbility* AbilityCDO = Cast<UMGameplayAbility>(ShopItem->GetGrantedAbilityCDO());
	if (!AbilityCDO) return 0.0f;
	
	return UMAbilitySystemStatics::GetManaCostFor(AbilityCDO, *CachedASC, 1);
}

bool UInventoryItem::CanCastAbility() const
{
	if (!CachedASC || !GrantedAbilitySpecHandle.IsValid()) return false;

	FGameplayAbilitySpec* Spec = CachedASC->FindAbilitySpecFromHandle(GrantedAbilitySpecHandle);
	if (!Spec || !Spec->Ability) return false;

	return Spec->Ability->CanActivateAbility(GrantedAbilitySpecHandle, CachedASC->AbilityActorInfo.Get());
}

void UInventoryItem::ApplyGASModifications()
{
	if (!ShopItem || !CachedASC) return;
	if (!CachedASC->GetOwner()->HasAuthority()) return;

	if (TSubclassOf<UGameplayEffect> EquipEF =ShopItem->GetEquippedEffect())
		AppliedEquippedEffectHandle=CachedASC->BP_ApplyGameplayEffectToSelf(EquipEF, 1 ,CachedASC->MakeEffectContext());

	if (TSubclassOf<UGameplayAbility> GrantedAbility=ShopItem->GetGrantedAbility())
		GrantedAbilitySpecHandle=CachedASC->GiveAbility(FGameplayAbilitySpec(GrantedAbility));
}

void UInventoryItem::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	OnAbilityCanCastUpdated.Broadcast(CanCastAbility());
}
