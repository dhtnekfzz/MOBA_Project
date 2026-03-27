// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GAP_Launched.h"

#include "MGameplayTags.h"

UGAP_Launched::UGAP_Launched()
{
	NetExecutionPolicy=EGameplayAbilityNetExecutionPolicy::ServerOnly;
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource=EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag=MGameplayTags::Ability_Passive_Launch_Activate;

	ActivationBlockedTags.RemoveTag(MGameplayTags::Ability_Passive_Launch_Activate);
	AbilityTriggers.Add(TriggerData);
}

void UGAP_Launched::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_HasAuthority())
	{
		K2_EndAbility();
		return;
	}

	if (K2_HasAuthority())
	{
		PushSelf(TriggerEventData->TargetData.Get(0)->GetHitResult()->ImpactNormal);
		K2_EndAbility();
	}
}
