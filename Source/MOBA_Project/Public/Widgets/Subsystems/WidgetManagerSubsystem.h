// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "WidgetManagerSubsystem.generated.h"

class UPrimaryLayout;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UWidgetManagerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void OnPlayerControllerChanged(APlayerController* NewPC);
	
	void CreatePrimaryLayout(APlayerController* PC, TSubclassOf<UPrimaryLayout> NewPrimaryLayoutClass);
	void ShowGameplayWidget(TSubclassOf<UCommonActivatableWidget> GameplayWidgetClass);
	void ShowRespawnWidget(TSubclassOf<UCommonActivatableWidget> SpawnWidgetClass, float RespawnTime);

	UCommonActivatableWidget* PushMenuWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass);
	
	void PopMenuWidget();
	void ClearMenuStack();

	void ToggleMenuWidget(TSubclassOf<UCommonActivatableWidget> ShopWidgetClass);

	bool IsMenuOpen() const;

	void PushOverlayWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass);

	void PopOverlayWidget();

	UPrimaryLayout* GetPrimaryLayout() const { return PrimaryLayout; };

private:
	UPROPERTY()
	TObjectPtr<UPrimaryLayout> PrimaryLayout;
	

	
	
};

