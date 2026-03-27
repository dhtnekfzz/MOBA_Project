// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MTypes/MStructTypes.h"
#include "MPlayerTeamLayoutWidget.generated.h"

class UMPlayerTeamSlotWidget;


UCLASS()
class MOBA_PROJECT_API UMPlayerTeamLayoutWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelections);

private:
	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* TeamOneLayoutBox;

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* TeamTwoLayoutBox;

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	TSubclassOf<UMPlayerTeamSlotWidget> PlayerTeamSlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	float SlotMargin = 5.f;

	UPROPERTY()
	TArray<TObjectPtr<UMPlayerTeamSlotWidget>> TeamSlotWidgets;
};
