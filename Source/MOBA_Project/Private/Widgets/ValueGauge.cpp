// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ValueGauge.h"

void UValueGauge::NativeConstruct()
{
	Super::NativeConstruct();
}

void UValueGauge::SetAndBoundToGameplayAttribute(class UAbilitySystemComponent* ASC,
	const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
}

void UValueGauge::SetValue(float NewValue, float NewMaxValue)
{
}

void UValueGauge::ValueChanged(const FOnAttributeChangeData& ChangedData)
{
}

void UValueGauge::MaxValueChanged(const FOnAttributeChangeData& ChangedData)
{
}
