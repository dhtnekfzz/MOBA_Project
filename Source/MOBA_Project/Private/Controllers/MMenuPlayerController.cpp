// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/MMenuPlayerController.h"
#include "Widgets/PrimaryLayout.h"
#include "Widgets/Subsystems/WidgetManagerSubsystem.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"


void AMMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	
	if (HasAuthority() && IsLocalPlayerController())
		InitializeMenuLayout();
}


void AMMenuPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (IsLocalPlayerController())
		InitializeMenuLayout();
}

void AMMenuPlayerController::AcknowledgePossession(class APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);

	if (NewPawn)
	{
		for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
		{
			SetViewTarget(*It);
			return;
		}
	}
}


void AMMenuPlayerController::InitializeMenuLayout()
{
	UWidgetManagerSubsystem* WM = GetLocalPlayer()->GetSubsystem<UWidgetManagerSubsystem>();
	if (!WM) return;

	WM->CreatePrimaryLayout(this, MenuLayoutClass);
	
	if (FirstMenuWidgetClass)
		WM->PushMenuWidget(FirstMenuWidgetClass);
	
}
