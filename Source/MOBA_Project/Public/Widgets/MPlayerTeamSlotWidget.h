// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MPlayerTeamSlotWidget.generated.h"

class UDA_MCharacterDefinition;

UCLASS()
class MOBA_PROJECT_API UMPlayerTeamSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	void UpdateSlot(const FString& PlayerName, const UDA_MCharacterDefinition* CharacterDefinition);

private:
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	class UWidgetAnimation* HoverAnim;

	UPROPERTY(meta=(BindWidget))
	class UImage* PlayerCharacterIcon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* NameText;

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName CharacterIconParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName EmptyParamName = "Empty";

	FString CachedPlayerName;
	FString CachedCharacterName;

	void RefreshNameText();
};
