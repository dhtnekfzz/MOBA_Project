// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/MGameplayAbility.h"
#include "GA_Combo.generated.h"

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UGA_Combo : public UMGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Combo();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effect")
	TMap<int32, TSubclassOf<UGameplayEffect>> DamageEffectMap;

	TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo() const;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TMap<int32, TObjectPtr<UAnimMontage>> ComboMontageMap;

	UFUNCTION()
	void DoDamage(FGameplayEventData Data);
	
	UPROPERTY(EditDefaultsOnly, Category="Combo")
	float ComboResetTime=0.3f;
	
	int32 ComboCount=0;
	float LastAttackTime=-9999.0f;

	
	UPROPERTY()
	TObjectPtr<class UAbilityTask_PlayMontageAndWait> CurrentMontageTask;

	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitGameplayEvent> CurrentWaitEventTask;
	
};


