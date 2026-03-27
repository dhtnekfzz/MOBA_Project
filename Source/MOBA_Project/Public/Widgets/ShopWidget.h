// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/MActivatableWidget.h"
#include "ShopWidget.generated.h"

class UTileView;
class UShopItemWidget;
class UDataAsset_ShopItem;
class UInventoryComponent;
class UShopSubsystem;
class UMItemTreeWidget;
class UShopItemTreeNode;
class UMVVM_ShopItem;


UCLASS()
class MOBA_PROJECT_API UShopWidget : public UMActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct()  override;
	
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override
	{
		return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture);
	}

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTileView> ShopItemList;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UMItemTreeWidget> CombinationTree;


	void OnShopItemsLoaded();

	void OnEntryWidgetGenerated(UUserWidget& Widget);
	
	void ShowItemCombination(const UShopItemWidget* NewWidget);


	UPROPERTY()
	TObjectPtr<UInventoryComponent> CachedInventoryComponent;

	UPROPERTY()
	TObjectPtr<UShopItemTreeNode> RootTreeNode;
};
