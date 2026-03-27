// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ShopItemWidget.h"
#include "Components/ListView.h"
#include "View/MVVMView.h"
#include "Widgets/MVVM/MVVM_ShopItem.h"
#include "Widgets/Subsystems/ShopSubsystem.h"
#include "DataAssets/DataAsset_ShopItem.h"
#include "Framework/MAssetManager.h"

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	ParentListView = Cast<UListView>(IUserListEntry::GetOwningListView());

	UMVVM_ShopItem* VM = Cast<UMVVM_ShopItem>(ListItemObject);
	if (!VM) return;
	
	InitWithViewModel(VM);
}

void UShopItemWidget::CopyFromOther(const UShopItemWidget* Source)
{
	if (!Source) return;

	OnItemPurchaseIssued = Source->OnItemPurchaseIssued;
	OnShopItemSelected   = Source->OnShopItemSelected;
	ParentListView       = Source->ParentListView;

	InitWithViewModel(Source->CachedVM);
}


void UShopItemWidget::InitWithViewModel(UMVVM_ShopItem* InVM)
{
	CachedVM = InVM;
	if (!CachedVM) return;
	
	SetToolTipWidget(CachedVM->GetShopItemData());
	
	if (UMVVMView* View = GetExtension<UMVVMView>())
		View->SetViewModel(FName("ShopVM"), CachedVM);
}

UUserWidget* UShopItemWidget::GetWidget() const
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return nullptr;

	UShopItemWidget* Copy = CreateWidget<UShopItemWidget>(PC, GetClass());
	if (Copy)
		Copy->CopyFromOther(this);

	return Copy;
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetInputs() const
{
	if (CachedVM)
	{
		const FItemCollection* Collection = UMAssetManager::Get().GetCombinationForItem(CachedVM->GetShopItemData());
		if (Collection)
			return ItemsToInterfaces(Collection->GetItems());
	}

	return TArray<const ITreeNodeInterface*>();
}

TArray<const ITreeNodeInterface*> UShopItemWidget::GetOuputs() const
{
	if (CachedVM)
	{
		const FItemCollection* Collection = UMAssetManager::Get().GetIngredientForItem(CachedVM->GetShopItemData());
		if (Collection)
			return ItemsToInterfaces(Collection->GetItems());
	}

	return TArray<const ITreeNodeInterface*>();
}

const UObject* UShopItemWidget::GetItemObject() const
{
	return CachedVM ? CachedVM->GetShopItemData() : nullptr;
}


TArray<const ITreeNodeInterface*> UShopItemWidget::ItemsToInterfaces(const TArray<const UDataAsset_ShopItem*>& Items) const
{
	TArray<const ITreeNodeInterface*> Result;
	if (!ParentListView) return Result;

	ULocalPlayer* LP = GetOwningLocalPlayer();
	UShopSubsystem* Shop = LP ? LP->GetSubsystem<UShopSubsystem>() : nullptr;
	if (!Shop) return Result;

	Result.Reserve(Items.Num());
	for (const UDataAsset_ShopItem* Item : Items)
	{
		if (!Item) continue;
		UMVVM_ShopItem* VM = Shop->GetViewModelForItem(Item);
		if (!VM) continue;
		const UShopItemWidget* Widget =
			ParentListView->GetEntryWidgetFromItem<UShopItemWidget>(VM);
		if (Widget)
			Result.Add(Widget);
	}
	return Result;
}

const UDataAsset_ShopItem* UShopItemWidget::GetShopItemData() const
{
	return CachedVM ? CachedVM->GetShopItemData() : nullptr;
}

void UShopItemWidget::PurchaseButtonClicked()
{
	OnItemPurchaseIssued.Broadcast(GetShopItemData());
}

void UShopItemWidget::SelectButtonClicked()
{
	OnShopItemSelected.Broadcast(this);
}


void UShopItemWidget::RightButtonClicked()
{
	PurchaseButtonClicked();
}

void UShopItemWidget::LeftButtonClicked()
{
	SelectButtonClicked();
}
