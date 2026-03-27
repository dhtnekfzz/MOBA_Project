// Fill out your copyright notice in the Description page of Project Settings.

// 부모 클래스가 UUserWidget → UMMenuActivatableWidget 으로 변경되었지만
// 내부 로직은 완전히 동일합니다.
// CommonUI Stack 에 Push 되는 방식으로 생성되므로
// AddToViewport / AddToPlayerScreen 을 직접 호출하지 않습니다.

#include "Widgets/MMainMenuWidget.h"
#include "Framework/MGameInstance.h"
#include "Network/MNetStatics.h"
#include "Widgets/MWaitingWidget.h"
#include "Widgets/MSessionEntryWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"

void UMMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MGameInstance = GetGameInstance<UMGameInstance>();
	if (MGameInstance)
	{
		MGameInstance->OnLoginCompleted.AddUObject(this, &UMMainMenuWidget::OnLoginCompleted);
		MGameInstance->OnJoinSessionFailed.AddUObject(this, &UMMainMenuWidget::OnJoinSessionFailed);
		MGameInstance->OnGlobalSessionSearchCompleted.AddUObject(this, &UMMainMenuWidget::OnSessionSearchCompleted);
		
		MGameInstance->StartGlobalSessionSearch();
	}

	LoginBtn->OnClicked.AddDynamic(this, &UMMainMenuWidget::OnLoginBtnClicked);

	CreateSessionBtn->OnClicked.AddDynamic(this, &UMMainMenuWidget::OnCreateSessionBtnClicked);
	CreateSessionBtn->SetIsEnabled(false);

	NewSessionNameText->OnTextChanged.AddDynamic(this, &UMMainMenuWidget::OnNewSessionNameTextChanged);

	JoinSessionBtn->OnClicked.AddDynamic(this, &UMMainMenuWidget::OnJoinSessionBtnClicked);
	JoinSessionBtn->SetIsEnabled(false);
}

void UMMainMenuWidget::OnLoginBtnClicked()
{
#if WITH_EDITOR
	SwitchToWaitingWidget(FText::FromString("Logging In"), true).AddDynamic(this, &UMMainMenuWidget::SwitchToMainWidget);
	MGameInstance->EditorLoginWithNickname(NicknameText->GetText().ToString().TrimStartAndEnd());
#else 
	if (MGameInstance && !MGameInstance->IsLoggedIn() && !MGameInstance->IsLoggingIn())
	{
		MGameInstance->ClientAccountPortalLogin();
		SwitchToWaitingWidget(FText::FromString("Logging In"));
	}
#endif
}

void UMMainMenuWidget::OnLoginCompleted(bool bWasSuccessful, const FString& Nickname, const FString& Error)
{
	if (bWasSuccessful)
		SwitchToMainWidget();
	else
		MainSwitcher->SetActiveWidget(LoginWidgetRoot);
}

void UMMainMenuWidget::OnCreateSessionBtnClicked()
{
	if (!MGameInstance || !MGameInstance->IsLoggedIn()) return;

	FName SessionName = FName(*NewSessionNameText->GetText().ToString());
	MGameInstance->RequestCreateAndJoinSession(SessionName);
	SwitchToWaitingWidget(FText::FromString("Creating Lobby"), true).AddDynamic(this, &UMMainMenuWidget::OnCancelSessionCreation);
}

void UMMainMenuWidget::OnCancelSessionCreation()
{
	if (MGameInstance) MGameInstance->CancelSessionCreation();
	SwitchToMainWidget();
}

void UMMainMenuWidget::OnNewSessionNameTextChanged(const FText& NewText)
{
	CreateSessionBtn->SetIsEnabled(!NewText.IsEmpty());
}

void UMMainMenuWidget::OnJoinSessionBtnClicked()
{
	if (!MGameInstance || CurrentSelectedSessionId.IsEmpty()) return;

	if (MGameInstance->JoinSessionWithId(CurrentSelectedSessionId))
		SwitchToWaitingWidget(FText::FromString("Joining"));
}

void UMMainMenuWidget::OnJoinSessionFailed()
{
	SwitchToMainWidget();
}

void UMMainMenuWidget::OnSessionSearchCompleted(const TArray<FOnlineSessionSearchResult>& Results)
{
	SessionScrollBox->ClearChildren();

	bool bCurrentSelectedSessionValid = false;
	for (const FOnlineSessionSearchResult& Result : Results)
	{
		UMSessionEntryWidget* Entry=CreateWidget<UMSessionEntryWidget>(GetOwningPlayer(), SessionEntryWidgetClass);
		if (!Entry) continue;

		FString Name = "None";
		Result.Session.SessionSettings.Get<FString>(UMNetStatics::GetSessionNameKey(), Name);
		FString IdStr = Result.GetSessionIdStr();

		Entry->InitializeEntry(Name, IdStr);
		Entry->OnMSessionEntrySelected.AddUObject(this, &UMMainMenuWidget::OnSessionEntrySelected);
		SessionScrollBox->AddChild(Entry);

		if (CurrentSelectedSessionId == IdStr)
			bCurrentSelectedSessionValid = true;
	}

	if (!bCurrentSelectedSessionValid)
		CurrentSelectedSessionId = "";

	JoinSessionBtn->SetIsEnabled(bCurrentSelectedSessionValid);
}

void UMMainMenuWidget::OnSessionEntrySelected(const FString& SelectedSessionId)
{
	CurrentSelectedSessionId = SelectedSessionId;
	JoinSessionBtn->SetIsEnabled(true);
}

void UMMainMenuWidget::SwitchToMainWidget()
{
	if (MainSwitcher && MainWidgetRoot)
		MainSwitcher->SetActiveWidget(MainWidgetRoot);
}

FOnButtonClickedEvent& UMMainMenuWidget::SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel)
{
	MainSwitcher->SetActiveWidget(WaitingWidget);
	WaitingWidget->SetWaitInfo(WaitInfo, bAllowCancel);
	return WaitingWidget->ClearAndGetCancelClickedEvent();
}
