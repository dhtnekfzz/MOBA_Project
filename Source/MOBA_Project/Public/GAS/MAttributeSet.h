// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GAS/MAbilitySystemComponent.h"
#include "MAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(UMAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UMAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UMAttributeSet, Mana)
	ATTRIBUTE_ACCESSORS(UMAttributeSet, MaxMana)
	ATTRIBUTE_ACCESSORS(UMAttributeSet, AttackDamage)
    ATTRIBUTE_ACCESSORS(UMAttributeSet, Armor)
    ATTRIBUTE_ACCESSORS(UMAttributeSet, MoveSpeed)
    ATTRIBUTE_ACCESSORS(UMAttributeSet, MoveAcceleration)
	ATTRIBUTE_ACCESSORS(UMAttributeSet, CachedHealthPercent);
	ATTRIBUTE_ACCESSORS(UMAttributeSet, CachedManaPercent);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	void RescaleHealth();
	void RescaleMana();
private:
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;

	UPROPERTY(ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;

	UPROPERTY(ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;

	UPROPERTY(ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;

	UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;

	UPROPERTY(ReplicatedUsing = OnRep_MoveAcceleration)
	FGameplayAttributeData MoveAcceleration;

	UPROPERTY()
	FGameplayAttributeData CachedHealthPercent;

	UPROPERTY()
	FGameplayAttributeData CachedManaPercent;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveAcceleration(const FGameplayAttributeData& OldValue);
	
};
