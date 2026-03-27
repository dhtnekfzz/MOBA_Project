// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_ActivateAbilityByTag.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "GAS/MAbilitySystemComponent.h"

EBTNodeResult::Type UBTTask_ActivateAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* OwnerAIC=OwnerComp.GetAIOwner();
	if (OwnerAIC)
	{
		UMAbilitySystemComponent* OwnerASC=Cast<UMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerAIC->GetPawn()));
		if (OwnerASC)
		{
			OwnerASC->OnAbilityInputPressed(AbilityTag);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
