// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PrimaryLayout.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UCommonActivatableWidget* UPrimaryLayout::PushGameWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!GameLayer||!WidgetClass) return nullptr;
	return GameLayer->AddWidget<UCommonActivatableWidget>(WidgetClass);
}

UCommonActivatableWidget* UPrimaryLayout::PushMenuWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!MenuLayer||!WidgetClass) return nullptr;
	return MenuLayer->AddWidget<UCommonActivatableWidget>(WidgetClass);

}

void UPrimaryLayout::PopMenuWidget()
{
	if (!MenuLayer) return;
	if (UCommonActivatableWidget* Active=MenuLayer->GetActiveWidget())
		MenuLayer->RemoveWidget(*Active);
}

void UPrimaryLayout::ClearMenuWidget()
{
	if (MenuLayer) MenuLayer->ClearWidgets();
}



bool UPrimaryLayout::IsMenuOpen() const
{
	return MenuLayer != nullptr && MenuLayer->GetActiveWidget() != nullptr;
}

UCommonActivatableWidget* UPrimaryLayout::PushOverlayWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!OverlayLayer || !WidgetClass) return nullptr;
	return OverlayLayer->AddWidget<UCommonActivatableWidget>(WidgetClass);
}

void UPrimaryLayout::PopOverlayWidget()
{
	if (!OverlayLayer) return;
	if (UCommonActivatableWidget* Active = OverlayLayer->GetActiveWidget())
		OverlayLayer->RemoveWidget(*Active);
}
