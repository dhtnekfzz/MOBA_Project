// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/MActivatableWidget.h"   // UUserWidget → UMMenuActivatableWidget
#include "Components/Button.h"
#include "MWaitingWidget.generated.h"

UCLASS()
class MOBA_PROJECT_API UMWaitingWidget : public UMMenuActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetWaitInfo(const FText& WaitInfo, bool bAllowCancel = false);
	FOnButtonClickedEvent& ClearAndGetCancelClickedEvent();

private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* WaitInfoText;

	UPROPERTY(meta=(BindWidget))
	class UButton* CancelBtn;
};
