// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MAbilitySystemStatics.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueManager.h"
#include "MGameplayTags.h"
#include "Abilities/GameplayAbility.h"

bool UMAbilitySystemStatics::IsHero(const AActor* Actor)
{
	if (ActorHasTag(Actor, MGameplayTags::Role_Hero)) return true;
	return false;
}

bool UMAbilitySystemStatics::ActorHasTag(const AActor* Actor, const FGameplayTag& Tag)
{
	const IAbilitySystemInterface* ActorISA=Cast<IAbilitySystemInterface>(Actor);

	if (ActorISA)
	{
		UAbilitySystemComponent* ActorASC=ActorISA->GetAbilitySystemComponent();
		if(ActorASC)
			return ActorASC->HasMatchingGameplayTag(Tag);
	}

	return false;
}

bool UMAbilitySystemStatics::IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec)
{
	return Spec.Level>=4;
}

float UMAbilitySystemStatics::GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability)
{
	if (!Ability) return 0.f;

	const UGameplayEffect* CooldownEffect = Ability->GetCooldownGameplayEffect();
	if (!CooldownEffect) return 0.f;

	float CooldownDuration = 0.f;

	CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration);
	return CooldownDuration;
	
}

float UMAbilitySystemStatics::GetStaticCostForAbility(const UGameplayAbility* Ability)
{
	if (!Ability) return 0.f;

	const UGameplayEffect* CostEffect = Ability->GetCostGameplayEffect();
	if (!CostEffect || CostEffect->Modifiers.Num() == 0) return 0.f;

	float Cost = 0.f;

	CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
	return FMath::Abs(Cost);
	
}

bool UMAbilitySystemStatics::CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec,
	const UAbilitySystemComponent& ASC)
{
	const UGameplayAbility* Ability=AbilitySpec.Ability;
	if (!Ability) return false;

	return Ability->CheckCost(AbilitySpec.Handle, ASC.AbilityActorInfo.Get());
}

bool UMAbilitySystemStatics::CheckAbilityCostStatic(const UGameplayAbility* AbilityCDO,
	const UAbilitySystemComponent& ASC)
{
	if (!AbilityCDO) return false;

	return AbilityCDO->CheckCost(FGameplayAbilitySpecHandle(), ASC.AbilityActorInfo.Get());
}

float UMAbilitySystemStatics::GetManaCostFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC,
	int AbilityLevel)
{
	float ManaCost=0.f;
	if (AbilityCDO)
	{
		UGameplayEffect* CostEffect=AbilityCDO->GetCostGameplayEffect();
		if (CostEffect)
		{
			FGameplayEffectSpecHandle EffectSpecHandle=ASC.MakeOutgoingSpec(CostEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CostEffect->Modifiers[0].ModifierMagnitude.AttemptCalculateMagnitude(*EffectSpecHandle.Data.Get(), ManaCost);
		}
	}

	return FMath::Abs(ManaCost);

}

float UMAbilitySystemStatics::GetCooldownDurationFor(const UGameplayAbility* AbilityCDO,
	const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float CooldownDuration = 0.f;
	if (AbilityCDO)
	{
		UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
		if (CooldownEffect)
		{
			FGameplayEffectSpecHandle EffectSpec = ASC.MakeOutgoingSpec(CooldownEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CooldownEffect->DurationMagnitude.AttemptCalculateMagnitude(*EffectSpec.Data.Get(), CooldownDuration);
		}
	}

	return FMath::Abs(CooldownDuration);
}

float UMAbilitySystemStatics::GetCooldownRemainingFor(const UGameplayAbility* AbilityCDO,
	const UAbilitySystemComponent& ASC)
{
	if (!AbilityCDO) return 0;

	UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
	if (!CooldownEffect) return 0;

	FGameplayEffectQuery CooldownEffectQuery;
	CooldownEffectQuery.EffectDefinition = CooldownEffect->GetClass();

	float CooldownRemaining = 0.f;
	FJsonSerializableArrayFloat CooldownTimeRemainings = ASC.GetActiveEffectsTimeRemaining(CooldownEffectQuery);

	for (float Remaining : CooldownTimeRemainings)
	{
		if (Remaining > CooldownRemaining)
		{
			CooldownRemaining = Remaining;
		}
	}

	return CooldownRemaining;	
}

void UMAbilitySystemStatics::SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult,
	const FGameplayTag& GameplayCueTag)
{
	FGameplayCueParameters CueParams;
	CueParams.Location = HitResult.ImpactPoint;
	CueParams.Normal = HitResult.ImpactNormal;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(CueTargetActor, GameplayCueTag, EGameplayCueEvent::Executed, CueParams);
}
