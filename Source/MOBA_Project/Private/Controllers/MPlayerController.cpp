// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/MPlayerController.h"
#include "Character/MPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Subsystems/PlayerStatsSubsystem.h"
#include "Widgets/Subsystems/ShopSubsystem.h"
#include "Widgets/Subsystems/WidgetManagerSubsystem.h"
#include "InventoryItem/InventoryComponent.h"
#include "Character/MPlayerCharacter.h"
#include "Widgets/ShopWidget.h"
#include "Widgets/MouseModeWidget.h"
#include "Widgets/GameplayWidget.h"
#include "Widgets/MGameplayMenuWidget.h"
#include "Widgets/Subsystems/CapturePointSubsystem.h"

void AMPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	MPlayerCharacter=Cast<AMPlayerCharacter>(InPawn);

	if (!MPlayerCharacter) return;
	MPlayerCharacter->ServerSideInit();
	MPlayerCharacter->SetGenericTeamId(TeamID);
}

void AMPlayerController::AcknowledgePossession(class APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	MPlayerCharacter=Cast<AMPlayerCharacter>(NewPawn);
	
	if (!MPlayerCharacter) return;
	MPlayerCharacter->ClientSideInit();

	SetInputMode(FInputModeGameOnly());
	
	if (UPlayerStatsSubsystem* Stats=GetLocalPlayer()->GetSubsystem<UPlayerStatsSubsystem>())
	{
		Stats->SetAbilityDataTable(AbilityDataTable);
		Stats->OnPawnPossessed(NewPawn);
	}
	
	if (UShopSubsystem* Shop = GetLocalPlayer()->GetSubsystem<UShopSubsystem>())
	{
		if (UInventoryComponent* InvComp = MPlayerCharacter->FindComponentByClass<UInventoryComponent>())
			Shop->BindToInventoryComponent(InvComp, InvComp->GetCapacity());

		if (UAbilitySystemComponent* ASC = MPlayerCharacter->GetAbilitySystemComponent())
			Shop->BindToASC(Cast<UMAbilitySystemComponent>(ASC));
		
		Shop->RequestShopItemsLoad();
	}

	if (UWidgetManagerSubsystem* WM=GetLocalPlayer()->GetSubsystem<UWidgetManagerSubsystem>())
	{
		WM->CreatePrimaryLayout(this,PrimaryLayoutClass);
		WM->ShowGameplayWidget(GameplayWidgetClass);
	}
	if (UCapturePointSubsystem* CPS = GetLocalPlayer()->GetSubsystem<UCapturePointSubsystem>())
		CPS->BindToCapturePoint();
}

void AMPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID=NewTeamID;
}

FGenericTeamId AMPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void AMPlayerController::ToggleShopWidget()
{
	if (UWidgetManagerSubsystem* WM = GetLocalPlayer()->GetSubsystem<UWidgetManagerSubsystem>())
		WM->ToggleMenuWidget(ShopWidgetClass);
}

void AMPlayerController::ToggleGameplayMenuWidget()
{
	if (UWidgetManagerSubsystem* WM = GetLocalPlayer()->GetSubsystem<UWidgetManagerSubsystem>())
		WM->ToggleMenuWidget(GameplayMenuWidgetClass);
}

void AMPlayerController::PushWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	if (UWidgetManagerSubsystem* WM = GetLocalPlayer()->GetSubsystem<UWidgetManagerSubsystem>())
		WM->PushMenuWidget(WidgetClass);
}

void AMPlayerController::EnableMouseMode()
{
	if (UWidgetManagerSubsystem* WM = GetLocalPlayer()->GetSubsystem<UWidgetManagerSubsystem>())
		WM->PushOverlayWidget(MouseModeWidgetClass);
	SetShowMouseCursor(true);
}

void AMPlayerController::DisableMouseMode()
{
	if (UWidgetManagerSubsystem* WM = GetLocalPlayer()->GetSubsystem<UWidgetManagerSubsystem>())
		WM->PopOverlayWidget();
	SetDisableHaptics(false);
}

void AMPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMPlayerController, TeamID);
}


void AMPlayerController::MatchFinished(AActor* ViewTarget, int32 WinningTeamId)
{
	if (!HasAuthority()) return;
	
	Client_MatchFinished(ViewTarget, WinningTeamId);
}

void AMPlayerController::Client_MatchFinished_Implementation(AActor* ViewTarget, int32 WinningTeamId)
{
	SetViewTargetWithBlend(ViewTarget, MatchFinishViewBlendTime);
	
	if (MPlayerCharacter)
		MPlayerCharacter->DisableInput(this);

	const bool bWon = (GetGenericTeamId().GetId() == (uint8)WinningTeamId);
	const FString ResultMsg = bWon ? TEXT("VICTORY!") : TEXT("DEFEAT...");
	
	if (UWidgetManagerSubsystem* WM =GetLocalPlayer()->GetSubsystem<UWidgetManagerSubsystem>())
	{
		WM->ClearMenuStack();
		if (UCommonActivatableWidget* Widget =WM->PushMenuWidget(GameplayMenuWidgetClass))
		{
			if (UMGameplayMenuWidget* MenuWidget = Cast<UMGameplayMenuWidget>(Widget))
			{
				MenuWidget->SetTitleText(ResultMsg);
			}
		}
	}
}


