// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "MTypes/MStructTypes.h"
#include "DataAsset_PlayerStartUpData.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UDataAsset_PlayerStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UMAbilitySystemComponent* InASC, int32 ApplyLevel=1) override;

	virtual TArray<FMPlayerAbilitySet> GetAbilitySet() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="StartUpData", meta=(TitleProperty="AbilityTag"))
	TArray<FMPlayerAbilitySet> PlayerStartUpAbilitySets;


};
