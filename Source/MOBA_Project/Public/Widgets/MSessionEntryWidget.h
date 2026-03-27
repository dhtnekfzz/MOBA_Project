// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MSessionEntryWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMSessionEntrySelected, const FString& /*SessionIdStr*/);

UCLASS()
class MOBA_PROJECT_API UMSessionEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	FOnMSessionEntrySelected OnMSessionEntrySelected;

	void InitializeEntry(const FString& Name, const FString& InSessionIdStr);

	FORCEINLINE FString GetCachedSessionIdStr() const { return CachedSessionIdStr; }

private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* SessionNameText;

	UPROPERTY(meta=(BindWidget))
	class UButton* SessionBtn;

	FString CachedSessionIdStr;

	UFUNCTION()
	void OnSessionBtnClicked();
};
