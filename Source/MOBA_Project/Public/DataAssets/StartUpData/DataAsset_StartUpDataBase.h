// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "Engine/DataAsset.h"
#include "MTypes/MStructTypes.h"
#include "DataAsset_StartUpDataBase.generated.h"

class UGameplayEffect;
class UMGameplayAbility;
class UMAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UMAbilitySystemComponent* InASC, int32 ApplyLevel=1);
	virtual void ApplyFullStatEffect(UMAbilitySystemComponent* InASC);
	void AuthApplyGameplayEffect(UMAbilitySystemComponent* InASC, TSubclassOf<UGameplayEffect> GameplayEffect, int Level=1);

	FORCEINLINE TArray<TSubclassOf<UMGameplayAbility>> GetActiveAbilities() const { return ActiveAbilities; }
	FORCEINLINE TArray<TSubclassOf<UMGameplayAbility>> GetPassiveAbilities() const { return PassiveAbilities; }
	FORCEINLINE TArray<TSubclassOf<UGameplayEffect>> GetInitialEffects() const { return InitialEffects; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetFullStatEffect() const { return FullStatEffect; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDeathEffect() const { return DeathEffect; }
	FORCEINLINE UDataTable* GetBaseStatDataTable() const { return BaseStatDataTable; }
	const FRealCurve* GetExperienceCurve() const;

	virtual TArray<FMPlayerAbilitySet> GetAbilitySet();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TArray<TSubclassOf<UMGameplayAbility>> ActiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TArray<TSubclassOf<UMGameplayAbility>> PassiveAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStatEffect;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Base Stats")
	UDataTable* BaseStatDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Level")
	UCurveTable* ExperienceCurvesTable;

	UPROPERTY(EditDefaultsOnly, Category = "Level")
	FName ExperienceRowName = "ExperienceNeededToReachLevel";

	void GrantAbilities(const TArray<TSubclassOf<UMGameplayAbility>>& InAbilities, UMAbilitySystemComponent* InASC, int32 ApplyLevel=1);
	
};
