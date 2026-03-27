// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MVVM/MVVM_Gauge.h"

#include "Misc/MemStack.h"

float UMVVM_Gauge::GetPercent() const
{
	if (MaxValue<=0.0f) return 0.0f;
	return CurrentValue/MaxValue;
}

void UMVVM_Gauge::SetCurrentValue(float NewValue)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(CurrentValue, NewValue))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetPercent);
	}
}

void UMVVM_Gauge::SetMaxValue(float NewMaxValue)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(MaxValue, NewMaxValue))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetPercent);
	}
}
