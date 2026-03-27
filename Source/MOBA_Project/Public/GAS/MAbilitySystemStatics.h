// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MAbilitySystemStatics.generated.h"

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMAbilitySystemStatics : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()

public:
	static bool IsHero(const AActor* Actor);
	static bool ActorHasTag(const AActor* Actor, const FGameplayTag& Tag);
	static bool IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec);
	
	static float GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability);
	static float GetStaticCostForAbility(const UGameplayAbility* Ability);

	static bool CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec, const UAbilitySystemComponent& ASC);
	static bool CheckAbilityCostStatic(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC);
	static float GetManaCostFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel);
	static float GetCooldownDurationFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel);
	static float GetCooldownRemainingFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC);
	static void SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult, const FGameplayTag& GameplayCueTag);
	
};
