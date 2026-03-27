// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_HeroStats.generated.h"

class UMAbilitySystemComponent;
class UAbilitySystemComponent;
struct FOnAttributeChangeData;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMVVM_HeroStats : public UMVVMViewModelBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OwnerASC;
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	float AttackDamage;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float Armor;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float Intelligence;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float Strength;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float MoveSpeed;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float CurrentHealth;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float CurrentMana;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float MaxMana;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float Level;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float Experience;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float PrevLevelExperience;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float NextLevelExperience;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float ExperiencePercent;
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float UpgradePoint;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess=true))
	float Gold;
	
public:
	void UpdateExperience(const FOnAttributeChangeData& Data, UMAbilitySystemComponent* ASC);
	
	float GetAttackDamage() const { return AttackDamage; };
	float GetArmor() const { return Armor; }
	float GetIntelligence() const { return Intelligence; }
	float GetStrength() const { return Strength; }
	float GetMoveSpeed() const { return MoveSpeed; }
	float GetCurrentHealth() const { return CurrentHealth; }
	float GetMaxHealth() const { return MaxHealth; }
	float GetCurrentMana() const { return CurrentMana; }
	float GetMaxMana() const { return MaxMana; }
	float GetLevel() const { return Level; }
	float GetExperience() const { return Experience; }
	float GetPrevLevelExperience() const { return PrevLevelExperience; }
	float GetNextLevelExperience() const { return NextLevelExperience; }
	float GetExperiencePercent() const { return ExperiencePercent; }
	float GetUpgradePoint() const { return UpgradePoint; }
	float GetGold() const { return Gold; }
	
	void SetAttackDamage(float NewValue);
	void SetArmor(float NewValue);
	void SetIntelligence(float NewValue);
	void SetStrength(float NewValue);
	void SetMoveSpeed(float NewValue);
	void SetCurrentHealth(float NewValue);
	void SetMaxHealth(float NewValue);
	void SetCurrentMana(float NewValue);
	void SetMaxMana(float NewValue);
	void SetLevel(float NewValue);
	void SetExperience(float NewValue);
	void SetPrevLevelExperience(float NewValue);
	void SetNextLevelExperience(float NewValue);
	void SetExperiencePercent(float NewValue);
	void SetUpgradePoint(float NewValue);
	void SetGold(float NewValue);


	
};


