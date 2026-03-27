// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/CapturePointWidget.h"
#include "Widgets/MVVM/MVVM_CapturePoint.h"
#include "Widgets/Subsystems/CapturePointSubsystem.h"
#include "View/MVVMView.h"

void UCapturePointWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (UCapturePointSubsystem* Sub =GetOwningLocalPlayer()->GetSubsystem<UCapturePointSubsystem>())
	{
		InitializeWithViewModel(Sub->GetViewModel());
	}
}

void UCapturePointWidget::InitializeWithViewModel(UMVVM_CapturePoint* InViewModel)
{
	if (!InViewModel) return;

	if (UMVVMView* View = GetExtension<UMVVMView>())
		View->SetViewModel(FName("CapturePointVM"), InViewModel);
}
