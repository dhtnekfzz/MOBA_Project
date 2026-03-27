// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MVVM/MVVM_AbilityEntry.h"
#include "GAS/MAbilitySystemStatics.h"
#include "GAS/MAbilitySystemComponent.h"
#include "GAS/MAttributeSet.h"
#include "GAS/MHeroAttributeSet.h"


void UMVVM_AbilityEntry::Initialize(UMAbilitySystemComponent* ASC, UGameplayAbility* InAbilityCDO, const FAbilityWidgetData* DataTable, UTexture2D* InIcon)
{
	if (!ASC || !InAbilityCDO) return;

	CachedASC  = ASC;
	AbilityCDO = InAbilityCDO;
	
	ASC->AbilityCommittedCallbacks.AddUObject(this, &UMVVM_AbilityEntry::OnAbilityCommitted);
	ASC->AbilitySpecDirtiedCallbacks.AddUObject(this, &UMVVM_AbilityEntry::OnAbilitySpecUpdated);

	ASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetUpgradePointAttribute()).AddUObject(this, &UMVVM_AbilityEntry::OnUpgradePointChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData&) { RefreshCanCast(); });

	float Cooldown = UMAbilitySystemStatics::GetStaticCooldownDurationForAbility(InAbilityCDO);
	float Cost = UMAbilitySystemStatics::GetStaticCostForAbility(InAbilityCDO);
	
	SetIcon(DataTable->Icon.LoadSynchronous());
	SetCooldownDuration(Cooldown);
	SetManaCost(Cost);
	SetAbilityNameText(DataTable->AbilityName);
	SetAbilityDescriptionText(DataTable->Description);
	

	bool bFound;
	const float UpgradePoint = ASC->GetGameplayAttributeValue(
		UMHeroAttributeSet::GetUpgradePointAttribute(), bFound);
	if (bFound)
	{
		FOnAttributeChangeData Dummy;
		Dummy.NewValue = UpgradePoint;
		OnUpgradePointChanged(Dummy);
	}

}

void UMVVM_AbilityEntry::DeInitialize()
{
	if (!CachedASC) return;
	
	CachedASC->AbilityCommittedCallbacks.RemoveAll(this);
	CachedASC->AbilitySpecDirtiedCallbacks.RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetUpgradePointAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetManaAttribute()).RemoveAll(this);
	
}

void UMVVM_AbilityEntry::SetIcon(UTexture2D* NewIcon)
{
	UE_MVVM_SET_PROPERTY_VALUE(Icon, NewIcon);
}

void UMVVM_AbilityEntry::SetCooldownRemaining(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(CooldownRemaining, NewValue);
}

void UMVVM_AbilityEntry::SetCooldownDuration(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(CooldownDuration, NewValue);
}

void UMVVM_AbilityEntry::SetManaCost(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(ManaCost, NewValue);
}

void UMVVM_AbilityEntry::SetbCanCast(bool bNewCanCast)
{
	UE_MVVM_SET_PROPERTY_VALUE(bCanCast, bNewCanCast);
}

void UMVVM_AbilityEntry::SetbUpgradeAvailable(bool bNewUpgradeAvailable)
{
	UE_MVVM_SET_PROPERTY_VALUE(bUpgradeAvailable, bNewUpgradeAvailable);
}

void UMVVM_AbilityEntry::SetbCooldownTextVisible(bool bNewVisible)
{
	UE_MVVM_SET_PROPERTY_VALUE(bCooldownTextVisible, bNewVisible);
}

void UMVVM_AbilityEntry::SetAbilityNameText(FText NewText)
{
	UE_MVVM_SET_PROPERTY_VALUE(AbilityNameText, NewText);
}

void UMVVM_AbilityEntry::SetAbilityDescriptionText(FText NewText)
{
	UE_MVVM_SET_PROPERTY_VALUE(AbilityDescriptionText, NewText);
}

void UMVVM_AbilityEntry::OnAbilityCommitted(UGameplayAbility* Ability)
{
	if (!AbilityCDO) return;
	if (Ability->GetClass() != AbilityCDO->GetClass()) return;

	float Remaining, Duration;
	Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(), Remaining, Duration);

	StartCooldown(Remaining, Duration);
}

void UMVVM_AbilityEntry::OnAbilitySpecUpdated(const FGameplayAbilitySpec& Spec)
{
	if (!AbilityCDO) return;
	if (Spec.Ability != AbilityCDO) return;

	const UAbilitySystemComponent* ASC = CachedASC;
	if (!ASC) return;

	const bool bLearned = Spec.Level > 0;
	UE_MVVM_SET_PROPERTY_VALUE(AbilityLevel, (int32)Spec.Level);

	const float NewCooldown = UMAbilitySystemStatics::GetCooldownDurationFor(Spec.Ability, *ASC, Spec.Level);
	const float NewCost = UMAbilitySystemStatics::GetManaCostFor(Spec.Ability, *ASC, Spec.Level);

	UE_MVVM_SET_PROPERTY_VALUE(CooldownDuration, NewCooldown);
	UE_MVVM_SET_PROPERTY_VALUE(ManaCost, NewCost);

	CachedSpecHandle = Spec.Handle;

	RefreshCanCast();
}

void UMVVM_AbilityEntry::OnUpgradePointChanged(const FOnAttributeChangeData& Data)
{
	const bool bHasPoint = Data.NewValue > 0;

	const FGameplayAbilitySpec* Spec = GetAbilitySpec();
	if (Spec && UMAbilitySystemStatics::IsAbilityAtMaxLevel(*Spec))
	{
		UE_MVVM_SET_PROPERTY_VALUE(bUpgradeAvailable, false);
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(bUpgradeAvailable, bHasPoint);
	
}

void UMVVM_AbilityEntry::StartCooldown(float TimeRemaining, float Duration)
{
	UWorld* World = GetWorld();
	if (!World) return;

	UE_MVVM_SET_PROPERTY_VALUE(CooldownDuration, Duration);
	UE_MVVM_SET_PROPERTY_VALUE(CooldownRemaining, TimeRemaining);
	UE_MVVM_SET_PROPERTY_VALUE(bCooldownTextVisible, true);

	World->GetTimerManager().SetTimer(
		CooldownTimer, this,
		&UMVVM_AbilityEntry::FinishCooldown, TimeRemaining);

	World->GetTimerManager().SetTimer(
		CooldownTickTimer, this,
		&UMVVM_AbilityEntry::TickCooldown, CooldownTickInterval, true, 0.f);
}

void UMVVM_AbilityEntry::TickCooldown()
{
	const float NewRemaining = CooldownRemaining - CooldownTickInterval;
    const float NewPercent   = CooldownDuration > 0.f
        ? 1.f - NewRemaining / CooldownDuration : 1.f;

    UE_MVVM_SET_PROPERTY_VALUE(CooldownRemaining, FMath::Max(0.f, NewRemaining));
    UE_MVVM_SET_PROPERTY_VALUE(CooldownPercent, NewPercent);
}

void UMVVM_AbilityEntry::FinishCooldown()
{
	if (UWorld* World = GetWorld())
		World->GetTimerManager().ClearTimer(CooldownTickTimer);

	UE_MVVM_SET_PROPERTY_VALUE(CooldownRemaining, 0.f);
	UE_MVVM_SET_PROPERTY_VALUE(CooldownPercent, 1.f);
	UE_MVVM_SET_PROPERTY_VALUE(bCooldownTextVisible, false);

	RefreshCanCast();
}

void UMVVM_AbilityEntry::RefreshCanCast()
{
	const UAbilitySystemComponent* ASC = CachedASC;
	const FGameplayAbilitySpec* Spec   = GetAbilitySpec();

	bool bResult = bIsLearned;
	if (bResult && Spec && ASC)
		bResult = UMAbilitySystemStatics::CheckAbilityCost(*Spec, *ASC);

	UE_MVVM_SET_PROPERTY_VALUE(bCanCast, bResult);
}

const FGameplayAbilitySpec* UMVVM_AbilityEntry::GetAbilitySpec() const
{
	UMAbilitySystemComponent* ASC=CachedASC;
	if (!ASC||!AbilityCDO) return nullptr;

	if (CachedSpecHandle.IsValid()) return ASC->FindAbilitySpecFromHandle(CachedSpecHandle);

	return ASC->FindAbilitySpecFromClass(AbilityCDO->GetClass());
}

