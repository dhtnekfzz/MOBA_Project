// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ShopWidget.h"

#include "Components/TileView.h"
#include "InventoryItem/InventoryComponent.h"
#include "Widgets/MItemTreeWidget.h"
#include "Widgets/ShopItemTreeNode.h"
#include "Widgets/ShopItemWidget.h"
#include "Widgets/MVVM/MVVM_ShopItem.h"
#include "Widgets/Subsystems/ShopSubsystem.h"
#include "DataAssets/DataAsset_ShopItem.h"
#include "Engine/AssetManager.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* Pawn = GetOwningPlayerPawn())
		CachedInventoryComponent = Pawn->FindComponentByClass<UInventoryComponent>();

	OnShopItemsLoaded();
	ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &ThisClass::OnEntryWidgetGenerated);
	
}

void UShopWidget::NativeDestruct()
{
	if (ShopItemList)
	{
		ShopItemList->OnItemClicked().RemoveAll(this);
		ShopItemList->OnEntryWidgetGenerated().RemoveAll(this);
	}

	RootTreeNode = nullptr;
	Super::NativeDestruct();
}


void UShopWidget::OnShopItemsLoaded()
{
	if (!ShopItemList) return;
	
	UShopSubsystem* Shop = GetOwningLocalPlayer()->GetSubsystem<UShopSubsystem>();
	if (!Shop) return;

	ShopItemList->ClearListItems();

	for (const TObjectPtr<UMVVM_ShopItem>& VM : Shop->GetShopItemViewModels())
	{
		if (VM)
			ShopItemList->AddItem(VM.Get());
	}
	
}

void UShopWidget::OnEntryWidgetGenerated(UUserWidget& Widget)
{
	UShopItemWidget* ItemWidget = Cast<UShopItemWidget>(&Widget);
	if (!ItemWidget) return;

	if (CachedInventoryComponent)
	{
		ItemWidget->OnItemPurchaseIssued.RemoveAll(CachedInventoryComponent.Get());
		ItemWidget->OnItemPurchaseIssued.AddUObject(CachedInventoryComponent.Get(), &UInventoryComponent::TryPurchase);
	}

	ItemWidget->OnShopItemSelected.AddUObject(this, &UShopWidget::ShowItemCombination);
}

void UShopWidget::ShowItemCombination(const UShopItemWidget* NewWidget)
{
	CombinationTree->DrawFromNode(NewWidget);
}


