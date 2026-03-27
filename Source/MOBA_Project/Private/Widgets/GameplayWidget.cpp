// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameplayWidget.h"
#include "Widgets/AbilityListView.h"
#include "Widgets/Subsystems/PlayerStatsSubsystem.h"
#include "Widgets/Subsystems/WidgetManagerSubsystem.h"

void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UPlayerStatsSubsystem* StatsSub=GetOwningLocalPlayer()->GetSubsystem<UPlayerStatsSubsystem>();
	if (!StatsSub) return;

	if(!AbilityListView) return;

	AbilityListView->ConfigureWithViewModels(StatsSub->GetAbilityViewModels());
	
}

void UGameplayWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UGameplayWidget::OnToggleShop()
{
	if (auto* WM=GetWidgetManager())
	{
		WM->ToggleMenuWidget(ShopWidgetClass);
	}
}

void UGameplayWidget::OnToggleGameplayMenu()
{
	if (auto* WM=GetWidgetManager())
		WM->PushMenuWidget(GameplayMenuWidgetClass);
}

void UGameplayWidget::PlayShopPopUpAnimation(bool bPlayForward)
{
}
