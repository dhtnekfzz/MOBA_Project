// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "InventoryContextMenuWidget.generated.h"

UCLASS()
class MOBA_PROJECT_API UInventoryContextMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnButtonClickedEvent& GetSellButtonClickedEvent() const;
	FOnButtonClickedEvent& GetUseButtonClickedEvent() const;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> UseButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> SellButton;
};
