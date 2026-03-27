// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/MActivatableWidget.h"   
#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "MMainMenuWidget.generated.h"

UCLASS()
class MOBA_PROJECT_API UMMainMenuWidget : public UMMenuActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* MainSwitcher;

	UPROPERTY(meta=(BindWidget))
	class UWidget* MainWidgetRoot;

	UPROPERTY(meta=(BindWidget))
	class UWidget* LoginWidgetRoot;

	UPROPERTY(meta=(BindWidget))
	class UMWaitingWidget* WaitingWidget;

	UPROPERTY(meta=(BindWidget))
	class UButton* LoginBtn;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* NicknameText;

	UFUNCTION()
	void OnLoginBtnClicked();

	void OnLoginCompleted(bool bWasSuccessful, const FString& Nickname, const FString& Error);

	UPROPERTY(meta=(BindWidget))
	class UEditableText* NewSessionNameText;

	UPROPERTY(meta=(BindWidget))
	class UButton* CreateSessionBtn;

	UFUNCTION()
	void OnCreateSessionBtnClicked();

	UFUNCTION()
	void OnCancelSessionCreation();

	UFUNCTION()
	void OnNewSessionNameTextChanged(const FText& NewText);
	
	UPROPERTY(meta=(BindWidget))
	class UScrollBox* SessionScrollBox;

	UPROPERTY(meta=(BindWidget))
	class UButton* JoinSessionBtn;

	UPROPERTY(EditDefaultsOnly, Category="Session")
	TSubclassOf<class UMSessionEntryWidget> SessionEntryWidgetClass;

	FString CurrentSelectedSessionId;

	UFUNCTION()
	void OnJoinSessionBtnClicked();

	void OnSessionEntrySelected(const FString& SelectedSessionId);
	void OnSessionSearchCompleted(const TArray<FOnlineSessionSearchResult>& Results);
	void OnJoinSessionFailed();

	UFUNCTION()
	void SwitchToMainWidget();

	UFUNCTION()
	FOnButtonClickedEvent& SwitchToWaitingWidget(const FText& WaitInfo, bool bAllowCancel = false);

	UPROPERTY()
	TObjectPtr<class UMGameInstance> MGameInstance;
};
