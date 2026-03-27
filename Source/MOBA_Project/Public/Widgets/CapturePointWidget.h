// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CapturePointWidget.generated.h"

class UMVVM_CapturePoint;


UCLASS()
class MOBA_PROJECT_API UCapturePointWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWithViewModel(UMVVM_CapturePoint* InViewModel);

protected:
	virtual void NativeConstruct() override;


};
