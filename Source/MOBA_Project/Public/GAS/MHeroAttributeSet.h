// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MHeroAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMHeroAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, Intelligence)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, Strength)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, Experience)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, PrevLevelExperience)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, NextLevelExperience)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, Level)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, UpgradePoint)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, MaxLevel)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, MaxLevelExperience)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, Gold)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, StrengthGrowthRate)
    ATTRIBUTE_ACCESSORS(UMHeroAttributeSet, IntelligenceGrowthRate)
	
	virtual void GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const override;
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_Intelligence)
	FGameplayAttributeData Intelligence;

	UPROPERTY(ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;
	
	UPROPERTY(ReplicatedUsing = OnRep_Experience)
	FGameplayAttributeData Experience;

	UPROPERTY()
	FGameplayAttributeData StrengthGrowthRate;
	
	UPROPERTY()
	FGameplayAttributeData IntelligenceGrowthRate;

	UPROPERTY(ReplicatedUsing = OnRep_PrevLevelExperience)
	FGameplayAttributeData PrevLevelExperience;

	UPROPERTY(ReplicatedUsing = OnRep_NextLevelExperience)
	FGameplayAttributeData NextLevelExperience;

	UPROPERTY(ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;
	
	UPROPERTY(ReplicatedUsing = OnRep_UpgradePoint)
	FGameplayAttributeData UpgradePoint;
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxLevel)
	FGameplayAttributeData MaxLevel;

	UPROPERTY(ReplicatedUsing = OnRep_MaxLevelExperience)
	FGameplayAttributeData MaxLevelExperience;

	UPROPERTY(ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Experience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PrevLevelExperience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_NextLevelExperience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_UpgradePoint(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxLevel(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxLevelExperience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Gold(const FGameplayAttributeData& OldValue);
	
};
