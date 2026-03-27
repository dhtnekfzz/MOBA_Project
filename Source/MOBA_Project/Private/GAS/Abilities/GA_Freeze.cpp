// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Freeze.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/TargetActor_GroundPick.h"
#include "MGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor_Around.h"

UGA_Freeze::UGA_Freeze()
{
	ActivationOwnedTags.AddTag(MGameplayTags::State_Aim);	
}

void UGA_Freeze::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;

	UAbilityTask_PlayMontageAndWait* PlayTargetingMontage=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	PlayTargetingMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayTargetingMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayTargetingMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayTargetingMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
	PlayTargetingMontage->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitTargetData=UAbilityTask_WaitTargetData::WaitTargetData(this,NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitTargetData->ValidData.AddDynamic(this, &ThisClass::TargetReceived);
	WaitTargetData->Cancelled.AddDynamic(this, &ThisClass::TargetingCancelled);
	WaitTargetData->ReadyForActivation();
	
	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);
	
	ATargetActor_GroundPick* GroundPickActor=Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickActor)
	{
		GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickActor->SetTargetAreaRadius(TargetingRadius);
		GroundPickActor->SetTargetTraceRange(TargetRange);
	}

	WaitTargetData->FinishSpawningActor(this, TargetActor);
	
	
}

void UGA_Freeze::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (K2_HasAuthority())
		BP_ApplyGameplayEffectToTarget(TargetDataHandle, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));

	FGameplayCueParameters CueParameters;
	CueParameters.Location=UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
	CueParameters.RawMagnitude=TargetingRadius;

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(GameplayCueTag, CueParameters);

	PlayMontageLocally(CastMontage);
	K2_EndAbility();
	
}

void UGA_Freeze::TargetingCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}
