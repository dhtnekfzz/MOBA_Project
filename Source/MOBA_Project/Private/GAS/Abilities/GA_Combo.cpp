// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Combo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "MGameplayTags.h"

UGA_Combo::UGA_Combo()
{
	BlockAbilitiesWithTag.AddTag(MGameplayTags::Ability_BasicAttack);
	InstancingPolicy=EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	float CurrentTime=GetWorld()->GetTimeSeconds();

	if (CurrentTime-LastAttackTime<=ComboResetTime) ComboCount++;
	else ComboCount=0;

	if (ComboCount==ComboMontageMap.Num()) ComboCount=0;

	TObjectPtr<UAnimMontage>* ComboMontage = ComboMontageMap.Find(ComboCount);
	if (!ComboMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		if (CurrentMontageTask) CurrentMontageTask->EndTask();
		if (CurrentWaitEventTask) CurrentWaitEventTask->EndTask();
		
		CurrentMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, *ComboMontage);
		CurrentMontageTask->OnBlendOut.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		CurrentMontageTask->OnCancelled.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		CurrentMontageTask->OnCompleted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		CurrentMontageTask->OnInterrupted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		CurrentMontageTask->ReadyForActivation();

	}

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MGameplayTags::Ability_Combo_Damage);
		WaitTargetingEventTask->EventReceived.AddDynamic(this, &UGA_Combo::DoDamage);
		WaitTargetingEventTask->ReadyForActivation();
	}
	
	
}

void UGA_Combo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (CurrentMontageTask) { CurrentMontageTask->EndTask(); }
	if (CurrentWaitEventTask) { CurrentWaitEventTask->EndTask(); }

	LastAttackTime=GetWorld()->GetTimeSeconds();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);


}

TSubclassOf<UGameplayEffect> UGA_Combo::GetDamageEffectForCurrentCombo() const
{
	const TSubclassOf<UGameplayEffect>* DamageEffect=DamageEffectMap.Find(ComboCount);
	
	if (DamageEffect) return *DamageEffect;
	
	return DefaultDamageEffect;
}



void UGA_Combo::DoDamage(FGameplayEventData Data)
{
	int HitResultCount=UAbilitySystemBlueprintLibrary::GetDataCountFromTargetData(Data.TargetData);

	for (int i=0; i<HitResultCount; ++i)
	{
		FHitResult HitResult=UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data.TargetData,i);
		TSubclassOf<UGameplayEffect> GameplayEffect=GetDamageEffectForCurrentCombo();
		ApplyGameplayEffectToHitResultActor(HitResult, GameplayEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	}
}

