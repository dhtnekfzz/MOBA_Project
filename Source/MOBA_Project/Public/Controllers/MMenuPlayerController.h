// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMenuPlayerController.generated.h"

class UCommonActivatableWidget;
class UPrimaryLayout;


UCLASS()
class MOBA_PROJECT_API AMMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;

	virtual void AcknowledgePossession(class APawn* NewPawn) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UPrimaryLayout> MenuLayoutClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UCommonActivatableWidget> FirstMenuWidgetClass;

private:
	void InitializeMenuLayout();
};
