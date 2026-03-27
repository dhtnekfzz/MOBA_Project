// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MActivatableWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.generated.h"

class UAbilityListView;
class UAbilityGauge;
class UCapturePointWidget;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UGameplayWidget : public UMActivatableWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void OnToggleShop();
	void OnToggleGameplayMenu();
	

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCommonActivatableWidget> ShopWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCommonActivatableWidget> GameplayMenuWidgetClass;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UAbilityListView> AbilityListView;
	
	void PlayShopPopUpAnimation(bool bPlayForward);
	
};	
