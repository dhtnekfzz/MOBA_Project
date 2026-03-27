// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MActivatableWidget.generated.h"

class UPlayerStatsSubsystem;
class UWidgetManagerSubsystem;


UCLASS()
class MOBA_PROJECT_API UMActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	UWidgetManagerSubsystem* GetWidgetManager() const;
	UPlayerStatsSubsystem*   GetPlayerStats();

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override
	{
		return FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently);
	}
};



UCLASS()
class MOBA_PROJECT_API UMMenuActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	UWidgetManagerSubsystem* GetWidgetManager() const;

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override
	{
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	}
};
