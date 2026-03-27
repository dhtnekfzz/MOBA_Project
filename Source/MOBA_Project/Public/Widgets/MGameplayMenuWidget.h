// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Widgets/MActivatableWidget.h"
#include "MGameplayMenuWidget.generated.h"

UCLASS()
class MOBA_PROJECT_API UMGameplayMenuWidget : public UMActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetTitleText(const FString& NewTitle);

	FOnButtonClickedEvent& GetResumeClickedDelegate();

protected:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override
	{
		return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture);
	}

private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* MenuTitle;

	UPROPERTY(meta=(BindWidget))
	class UButton* ResumeBtn;

	UPROPERTY(meta=(BindWidget))
	class UButton* MainMenuBtn;

	UPROPERTY(meta=(BindWidget))
	class UButton* QuitGameBtn;

	UFUNCTION()
	void OnResumeBtnClicked();

	UFUNCTION()
	void OnMainMenuBtnClicked();

	UFUNCTION()
	void OnQuitGameBtnClicked();
};
