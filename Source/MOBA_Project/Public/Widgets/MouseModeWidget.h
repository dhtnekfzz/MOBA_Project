// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/MActivatableWidget.h"
#include "MouseModeWidget.generated.h"

UCLASS()
class MOBA_PROJECT_API UMouseModeWidget : public UMActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override
	{
		return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture);
	}

};
