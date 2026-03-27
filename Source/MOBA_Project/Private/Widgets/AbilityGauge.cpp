// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"

#include "View/MVVMView.h"
#include "Widgets/MVVM/MVVM_AbilityEntry.h"

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	CachedAbilityVM = Cast<UMVVM_AbilityEntry>(ListItemObject);
	if (!CachedAbilityVM) return;

	if (UMVVMView* View = GetExtension<UMVVMView>())
	{
		View->SetViewModel(FName("AbilityVM"), CachedAbilityVM);
	}

	CreateToolTipWidget();
}

void UAbilityGauge::CreateToolTipWidget()
{
	if (CachedAbilityVM)
	{
		if (GetOwningPlayer() || AbilityToolTipClass)
		{
			UUserWidget* ToolTip=CreateWidget(GetOwningPlayer(), AbilityToolTipClass);

			if (UMVVMView* View = ToolTip->GetExtension<UMVVMView>())
			{
				View->SetViewModel(FName("AbilityVM"), CachedAbilityVM);
			}
			SetToolTip(ToolTip);
		}
	}
	else
	{
		SetToolTip(nullptr);
	}
}
