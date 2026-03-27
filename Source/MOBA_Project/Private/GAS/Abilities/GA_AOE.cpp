// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_AOE.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor_Around.h"

UGA_AOE::UGA_AOE()
{
}

void UGA_AOE::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* MontageTask =UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AOEMontage);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_AOE::K2_EndAbility);
		MontageTask->OnBlendOut.AddDynamic(this, &UGA_AOE::K2_EndAbility);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_AOE::K2_EndAbility);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_AOE::K2_EndAbility);
		MontageTask->ReadyForActivation();
	}
	
	UAbilityTask_WaitGameplayEvent* WaitDetectEvent =UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MGameplayTags::Ability_AOE);
	WaitDetectEvent->EventReceived.AddDynamic(this, &UGA_AOE::OnDetectEvent);
	WaitDetectEvent->ReadyForActivation();
}


void UGA_AOE::OnDetectEvent(FGameplayEventData Payload)
{
	UAbilityTask_WaitTargetData* WaitTargetData =UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None,EGameplayTargetingConfirmation::CustomMulti, TargetActorClass);
	WaitTargetData->ValidData.AddDynamic(this, &UGA_AOE::TargetReceived);
	WaitTargetData->ReadyForActivation();

	AGameplayAbilityTargetActor* SpawnedActor;
	WaitTargetData->BeginSpawningActor(this, TargetActorClass, SpawnedActor);


	if (ATargetActor_Around* AOEActor = Cast<ATargetActor_Around>(SpawnedActor))
		AOEActor->ConfigureDetection(DetectionRadius, GetOwnerTeamId(), FGameplayTag());

	WaitTargetData->FinishSpawningActor(this, SpawnedActor);
	
	FGameplayCueParameters FreezeCueParameters;
	FreezeCueParameters.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	FreezeCueParameters.RawMagnitude = DetectionRadius;

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(AOEGameplayCueTag, FreezeCueParameters);
	
}
void UGA_AOE::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_HasAuthority()) return;
	
	PushTargetsFromOwnerLocation(TargetDataHandle, LaunchSpeed);
	BP_ApplyGameplayEffectToTarget(TargetDataHandle, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
}
