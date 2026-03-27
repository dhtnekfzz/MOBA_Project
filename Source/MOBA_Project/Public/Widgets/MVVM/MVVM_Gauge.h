// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_Gauge.generated.h"

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMVVM_Gauge : public UMVVMViewModelBase
{
	GENERATED_BODY()
	

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	float CurrentValue;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	float MaxValue;
ㅐ
public:
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetPercent() const;

	float GetCurrentValue() const {	return CurrentValue; }
	float GetMaxValue() const {	return MaxValue; }

	void SetCurrentValue(float NewValue);
	void SetMaxValue(float NewMaxValue);
	
};
