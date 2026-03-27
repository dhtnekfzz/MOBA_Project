// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PrimaryLayout.generated.h"

class UCommonActivatableWidgetStack;
class UCommonActivatableWidget;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UPrimaryLayout : public UUserWidget
{
	GENERATED_BODY()

public:
	UCommonActivatableWidget* PushGameWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass);
	UCommonActivatableWidget* PushMenuWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass);
	UCommonActivatableWidget* PushOverlayWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass);
	
	void PopMenuWidget();
	void PopOverlayWidget();
	void ClearMenuWidget();

	bool IsMenuOpen() const;

	FORCEINLINE UCommonActivatableWidgetStack* GetGameLayer() { return GameLayer; }
	FORCEINLINE UCommonActivatableWidgetStack* GetMenuLayer() { return MenuLayer; }
	FORCEINLINE UCommonActivatableWidgetStack* GetOverlayLayer() { return OverlayLayer; }

protected:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> GameLayer;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> MenuLayer;
	
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> OverlayLayer;
	
};
