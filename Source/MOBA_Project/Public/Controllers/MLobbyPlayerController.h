// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/MMenuPlayerController.h"
#include "MLobbyPlayerController.generated.h"

DECLARE_DELEGATE(FOnSwitchToHeroSelection);


UCLASS()
class MOBA_PROJECT_API AMLobbyPlayerController : public AMMenuPlayerController
{
	GENERATED_BODY()

public:
	AMLobbyPlayerController();

	virtual void BeginPlay() override;

	FOnSwitchToHeroSelection OnSwitchToHeroSelection;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSlotChange(uint8 NewSlotId);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartHeroSelection();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestStartMatch();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPlayerNickName(const FString& NickName);


private:
	UFUNCTION(Client, Reliable)
	void Client_StartHeroSelection();
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UCommonActivatableWidget> HeroSelectionWidgetClass;
};
