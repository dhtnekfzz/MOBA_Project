// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

class UImage;
class UMVVM_ShopItem;
class UDataAsset_ShopItem;

UCLASS()
class MOBA_PROJECT_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void SetIcon(UTexture2D* IconTexture) {};
	UUserWidget* SetToolTipWidget(const UDataAsset_ShopItem* Item);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void RightButtonClicked();
	virtual void LeftButtonClicked();

private:
	UPROPERTY(EditDefaultsOnly, Category="ToolTip")
	TSubclassOf<UUserWidget> ItemToolTipClass;


};
