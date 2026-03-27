// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MActivatableWidget.h"
#include "Widgets/Subsystems/WidgetManagerSubsystem.h"
#include "Widgets/Subsystems/PlayerStatsSubsystem.h"

// ── UMActivatableWidget ───────────────────────────────────────────────────────

UWidgetManagerSubsystem* UMActivatableWidget::GetWidgetManager() const
{
	return GetOwningLocalPlayer()->GetSubsystem<UWidgetManagerSubsystem>();
}

UPlayerStatsSubsystem* UMActivatableWidget::GetPlayerStats()
{
	return GetOwningLocalPlayer()->GetSubsystem<UPlayerStatsSubsystem>();
}

// ── UMMenuActivatableWidget ───────────────────────────────────────────────────

UWidgetManagerSubsystem* UMMenuActivatableWidget::GetWidgetManager() const
{
	return GetOwningLocalPlayer()->GetSubsystem<UWidgetManagerSubsystem>();
}
