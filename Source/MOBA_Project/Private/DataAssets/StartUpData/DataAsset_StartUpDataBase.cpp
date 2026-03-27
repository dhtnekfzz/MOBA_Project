// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/MAbilitySystemComponent.h"
#include "GAS/MAttributeSet.h"
#include "GAS/MHeroAttributeSet.h"
#include "GAS/Abilities/MGameplayAbility.h"
#include "MTypes/MStructTypes.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UMAbilitySystemComponent* InASC, int32 ApplyLevel)
{
	check(InASC);

	GrantAbilities(ActiveAbilities, InASC, ApplyLevel);
	GrantAbilities(PassiveAbilities, InASC, ApplyLevel);

	if (BaseStatDataTable)
	{
		const FMHeroBaseStats* BaseStats=nullptr;
	
		for (const TPair<FName, uint8*>& DataPair : BaseStatDataTable->GetRowMap())
		{
			BaseStats=BaseStatDataTable->FindRow<FMHeroBaseStats>(DataPair.Key, "");
			if (BaseStats && BaseStats->Class==InASC->GetOwner()->GetClass()) break;
		}
		
		if (BaseStats)
		{
			InASC->SetNumericAttributeBase(UMAttributeSet::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);
			InASC->SetNumericAttributeBase(UMAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);
			InASC->SetNumericAttributeBase(UMAttributeSet::GetAttackDamageAttribute(), BaseStats->BaseAttackDamage);
			InASC->SetNumericAttributeBase(UMAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);
			InASC->SetNumericAttributeBase(UMAttributeSet::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);

			InASC->SetNumericAttributeBase(UMHeroAttributeSet::GetStrengthAttribute(), BaseStats->Strength);
			InASC->SetNumericAttributeBase(UMHeroAttributeSet::GetStrengthGrowthRateAttribute(), BaseStats->StrengthGrowthRate);
			InASC->SetNumericAttributeBase(UMHeroAttributeSet::GetIntelligenceAttribute(), BaseStats->Intelligence);
			InASC->SetNumericAttributeBase(UMHeroAttributeSet::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowthRate);
		}
	}
	
	if (InitialEffects.IsEmpty()) return;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
	{
		AuthApplyGameplayEffect(InASC, EffectClass);
	}
}

void UDataAsset_StartUpDataBase::ApplyFullStatEffect(UMAbilitySystemComponent* InASC)
{
	if (!FullStatEffect) return;

	AuthApplyGameplayEffect(InASC, FullStatEffect);
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UMGameplayAbility>>& InAbilities,UMAbilitySystemComponent* InASC, int32 ApplyLevel)
{
	if (InAbilities.IsEmpty()) return;

	for (const TSubclassOf<UMGameplayAbility>& Ability : InAbilities)
	{
		if (!Ability) continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject=InASC->GetAvatarActor();
		AbilitySpec.Level=ApplyLevel;

		InASC->GiveAbility(AbilitySpec);
		
	}
}

void UDataAsset_StartUpDataBase::AuthApplyGameplayEffect(UMAbilitySystemComponent* InASC, TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	UGameplayEffect* EffectCDO=GameplayEffect->GetDefaultObject<UGameplayEffect>();
	InASC->ApplyGameplayEffectToSelf(EffectCDO, Level, InASC->MakeEffectContext());
}

const FRealCurve* UDataAsset_StartUpDataBase::GetExperienceCurve() const
{
	if (!ExperienceCurvesTable) return nullptr;
	return ExperienceCurvesTable->FindCurve(ExperienceRowName,"");
}

TArray<FMPlayerAbilitySet> UDataAsset_StartUpDataBase::GetAbilitySet()
{
	// override in child class
	return TArray<FMPlayerAbilitySet>();
}
