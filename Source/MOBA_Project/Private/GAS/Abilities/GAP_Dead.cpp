// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GAP_Dead.h"

#include "MGameplayTags.h"
#include "GAS/MAbilitySystemStatics.h"
#include "GAS/MHeroAttributeSet.h"
#include "Engine/OverlapResult.h"

UGAP_Dead::UGAP_Dead()
{
	NetExecutionPolicy=EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource=EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag=MGameplayTags::State_Dead;

	AbilityTriggers.Add(TriggerData);
	ActivationBlockedTags.RemoveTag(MGameplayTags::State_Stun);
}

void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_HasAuthority()) return;

	AActor* Killer=TriggerEventData->ContextHandle.GetEffectCauser();
	if (!Killer || UMAbilitySystemStatics::IsHero(Killer)) Killer=nullptr;

	TArray<AActor*> RewardTargets=GetRewardTargets();
	if (RewardTargets.IsEmpty() && !Killer)
	{
		K2_EndAbility();
		return;
	}

	if (Killer && !RewardTargets.Contains(Killer))
		RewardTargets.Add(Killer);

	bool bFound = false;
	float SelfExperience = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(UMHeroAttributeSet::GetExperienceAttribute(), bFound);

	float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;
	float TotalGoldReward = BaseGoldReward + GoldRewardPerExperience * SelfExperience;

	if (Killer)
	{
		float KillerExperienceReward = TotalExperienceReward * KillerRewardPortion;
		float KillerGoldReward = TotalGoldReward * KillerRewardPortion;

		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RewardEffect);
		EffectSpec.Data->SetSetByCallerMagnitude(MGameplayTags::Attr_Experience, KillerExperienceReward);
		EffectSpec.Data->SetSetByCallerMagnitude(MGameplayTags::Attr_Gold, KillerGoldReward);

		K2_ApplyGameplayEffectSpecToTarget(EffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));

		TotalExperienceReward -= KillerExperienceReward;
		TotalGoldReward -= KillerGoldReward;
	}

	float ExperiencePerTarget = TotalExperienceReward / RewardTargets.Num();
	float GoldPerTarget = TotalGoldReward / RewardTargets.Num();

	FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RewardEffect);
	EffectSpec.Data->SetSetByCallerMagnitude(MGameplayTags::Attr_Experience, ExperiencePerTarget);
	EffectSpec.Data->SetSetByCallerMagnitude(MGameplayTags::Attr_Gold, GoldPerTarget);

	K2_ApplyGameplayEffectSpecToTarget(EffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardTargets, true));
	K2_EndAbility();
	
}

TArray<AActor*> UGAP_Dead::GetRewardTargets() const
{
	TSet<AActor*> OutActors;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !GetWorld())
	{
		return OutActors.Array();
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(RewardRange);

	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByObjectType(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, ObjectQueryParams, CollisionShape))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			const IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OverlapResult.GetActor());
			if (!OtherTeamInterface || OtherTeamInterface->GetTeamAttitudeTowards(*AvatarActor) != ETeamAttitude::Hostile)
			{
				continue;
			}
			
			if (!UMAbilitySystemStatics::IsHero(OverlapResult.GetActor()))
			{
				continue;
			}

			OutActors.Add(OverlapResult.GetActor());
		}
	}

	return OutActors.Array();
}
