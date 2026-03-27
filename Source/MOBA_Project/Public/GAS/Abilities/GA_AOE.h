// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/MGameplayAbility.h"
#include "GA_AOE.generated.h"

class ATargetActor_Around;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UGA_AOE : public UMGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_AOE();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TObjectPtr<UAnimMontage> AOEMontage;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSubclassOf<ATargetActor_Around> TargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float DetectionRadius=400.f;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category="Launch")
	float LaunchSpeed=3000;

	UPROPERTY(EditDefaultsOnly, Category="GameplayCue")
	FGameplayTag AOEGameplayCueTag;

	UFUNCTION()
	void OnDetectEvent(FGameplayEventData Payload);

	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	
	
};



