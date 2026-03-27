// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MTypes/MStructTypes.h"
#include "GAS/MAbilitySystemComponent.h"
#include "MVVM_AbilityEntry.generated.h"

class UGameplayAbility;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMVVM_AbilityEntry : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void Initialize(UMAbilitySystemComponent* ASC, UGameplayAbility* InAbilityCDO, const FAbilityWidgetData* DataTable, UTexture2D* InIcon);
	void DeInitialize();

	UTexture2D* GetIcon() const { return Icon; }
	float GetCooldownRemaining() const { return CooldownRemaining; }
	float GetCooldownDuration() const { return CooldownDuration; }
	float GetCooldownPercent() const { return CooldownPercent; }
	float GetManaCost() const { return ManaCost; }
	int32 GetAbilityLevel() const { return AbilityLevel; }
	bool GetbCanCast() const { return bCanCast; }
	bool GetbIsLearned() const { return bIsLearned; }
	bool GetbUpgradeAvailable() const { return bUpgradeAvailable; }

	void SetIcon(UTexture2D* NewIcon);
	void SetCooldownRemaining(float NewValue);
	void SetCooldownDuration(float NewValue);
	void SetManaCost(float NewValue);
	void SetbCanCast(bool bNewCanCast);
	void SetbUpgradeAvailable(bool bNewUpgradeAvailable);
	void SetbCooldownTextVisible(bool bNewVisible);
	void SetAbilityNameText(FText NewText);
	void SetAbilityDescriptionText(FText NewText);

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	UTexture2D* Icon;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter,meta=(AllowPrivateAccess=true))
	float CooldownRemaining=0.f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter,meta=(AllowPrivateAccess=true))
	float CooldownDuration=0.f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess=true))
	float CooldownPercent=1.f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter,meta=(AllowPrivateAccess=true))
	float ManaCost=0.f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter, meta=(AllowPrivateAccess=true))
	int32 AbilityLevel=0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter,meta=(AllowPrivateAccess=true))
	bool bCanCast=false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Getter,meta=(AllowPrivateAccess=true))
	bool bIsLearned=false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter,meta=(AllowPrivateAccess=true))
	bool bUpgradeAvailable=false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter,meta=(AllowPrivateAccess=true) )
	bool bCooldownTextVisible=false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, meta=(AllowPrivateAccess=true) )
	FText AbilityNameText;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, meta=(AllowPrivateAccess=true) )
	FText AbilityDescriptionText;
	
private:
	void OnAbilityCommitted(UGameplayAbility* Ability);
	void OnAbilitySpecUpdated(const FGameplayAbilitySpec& Spec);
	void OnUpgradePointChanged(const FOnAttributeChangeData& Data);

	void StartCooldown(float TimeRemaining, float Duration);
	void TickCooldown();
	void FinishCooldown();
	void RefreshCanCast();

	const FGameplayAbilitySpec* GetAbilitySpec() const;

	UPROPERTY()
	UMAbilitySystemComponent* CachedASC;

	UPROPERTY()
	UGameplayAbility* AbilityCDO;

	FGameplayAbilitySpecHandle CachedSpecHandle;

	FTimerHandle CooldownTimer;
	FTimerHandle CooldownTickTimer;
	
	float CooldownTickInterval=0.1f;
	
	
};


