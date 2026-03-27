// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartUpData/DataAsset_PlayerStartUpData.h"

#include "GAS/MAbilitySystemComponent.h"

void UDataAsset_PlayerStartUpData::GiveToAbilitySystemComponent(UMAbilitySystemComponent* InASC, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASC, ApplyLevel);

	for (const FMPlayerAbilitySet& AbilitySet : PlayerStartUpAbilitySets)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.Ability);
		AbilitySpec.SourceObject=InASC->GetAvatarActor();
		AbilitySpec.Level=ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.AbilityTag);

		InASC->GiveAbility(AbilitySpec);
		
	}
}

TArray<FMPlayerAbilitySet> UDataAsset_PlayerStartUpData::GetAbilitySet()
{
	return PlayerStartUpAbilitySets;
}
