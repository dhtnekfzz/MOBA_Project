// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MTeamSelectionWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMSlotClicked, uint8 /*SlotID*/);

UCLASS()
class MOBA_PROJECT_API UMTeamSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FOnMSlotClicked OnMSlotClicked;

	void SetSlotID(uint8 NewSlotID);
	void UpdateSlotInfo(const FString& PlayerNickName);

private:
	UPROPERTY(meta=(BindWidget))
	class UButton* SelectButton;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* InfoText;

	uint8 SlotID = 0;

	UFUNCTION()
	void OnSelectButtonClicked();
};
