// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Subsystems/WidgetManagerSubsystem.h"
#include "Widgets/PrimaryLayout.h"
#include "Widgets/ShopWidget.h"

void UWidgetManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

void UWidgetManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}


void UWidgetManagerSubsystem::CreatePrimaryLayout(APlayerController* PC, TSubclassOf<UPrimaryLayout> NewPrimaryLayoutClass)
{
	if ( !PC || !NewPrimaryLayoutClass) return;
	
	PrimaryLayout=nullptr;

	PrimaryLayout=CreateWidget<UPrimaryLayout>(PC, NewPrimaryLayoutClass);
	PrimaryLayout->AddToViewport(0);
	
}

void UWidgetManagerSubsystem::ShowGameplayWidget(TSubclassOf<UCommonActivatableWidget> GameplayWidgetClass)
{
	if (PrimaryLayout)
		PrimaryLayout->PushGameWidget(GameplayWidgetClass);
}

void UWidgetManagerSubsystem::ShowRespawnWidget(TSubclassOf<UCommonActivatableWidget> SpawnWidgetClass, float RespawnTime)
{
	PushMenuWidget(SpawnWidgetClass);
}

UCommonActivatableWidget* UWidgetManagerSubsystem::PushMenuWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!PrimaryLayout) return nullptr;
	return PrimaryLayout->PushMenuWidget(WidgetClass);
}

void UWidgetManagerSubsystem::PopMenuWidget()
{
	if (PrimaryLayout) PrimaryLayout->PopMenuWidget();
}

void UWidgetManagerSubsystem::ClearMenuStack()
{
	if (PrimaryLayout) PrimaryLayout->ClearMenuWidget();
}

void UWidgetManagerSubsystem::ToggleMenuWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (!PrimaryLayout || !WidgetClass) return;

	UCommonActivatableWidget* ActiveWidget=PrimaryLayout->GetMenuLayer()->GetActiveWidget();

	if (!ActiveWidget)
	{
		PrimaryLayout->PushMenuWidget(WidgetClass);
		return;
	}
	
	if (ActiveWidget->GetClass()==WidgetClass)
		PrimaryLayout->PopMenuWidget();
	else
		PrimaryLayout->PushMenuWidget(WidgetClass);
}	

bool UWidgetManagerSubsystem::IsMenuOpen() const
{
	return PrimaryLayout && PrimaryLayout->IsMenuOpen();
}

void UWidgetManagerSubsystem::PushOverlayWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (PrimaryLayout) PrimaryLayout->PushOverlayWidget(WidgetClass);
}

void UWidgetManagerSubsystem::PopOverlayWidget()
{
	if (PrimaryLayout) PrimaryLayout->PopOverlayWidget();
}


