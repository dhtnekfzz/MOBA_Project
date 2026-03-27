// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "AbilityListView.generated.h"

class UMVVM_AbilityEntry;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UAbilityListView : public UListView
{
	GENERATED_BODY()

public:
	void ConfigureWithViewModels(const TArray<UMVVM_AbilityEntry*>& ViewModels);
	
};
