// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MGameplayMenuWidget.h"
#include "Widgets/Subsystems/WidgetManagerSubsystem.h"
#include "Framework/MGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"

void UMGameplayMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeBtn->OnClicked.AddDynamic(this, &UMGameplayMenuWidget::OnResumeBtnClicked);
	MainMenuBtn->OnClicked.AddDynamic(this, &UMGameplayMenuWidget::OnMainMenuBtnClicked);
	QuitGameBtn->OnClicked.AddDynamic(this, &UMGameplayMenuWidget::OnQuitGameBtnClicked);

	SetTitleText("PAUSED");
}


void UMGameplayMenuWidget::SetTitleText(const FString& NewTitle)
{
	if (MenuTitle)
		MenuTitle->SetText(FText::FromString(NewTitle));
}

FOnButtonClickedEvent& UMGameplayMenuWidget::GetResumeClickedDelegate()
{
	return ResumeBtn->OnClicked;
}


void UMGameplayMenuWidget::OnResumeBtnClicked()
{
	if (UWidgetManagerSubsystem* WM = GetWidgetManager())
		WM->PopMenuWidget();
}


void UMGameplayMenuWidget::OnMainMenuBtnClicked()
{
	GetOwningPlayer()->ClientTravel("/Game/MOBA/Maps/MainMenu/L_MainMenu", TRAVEL_Absolute);
}


void UMGameplayMenuWidget::OnQuitGameBtnClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
