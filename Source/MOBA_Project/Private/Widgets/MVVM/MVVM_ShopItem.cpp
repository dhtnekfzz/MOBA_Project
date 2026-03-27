// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MVVM/MVVM_ShopItem.h"
#include "DataAssets/DataAsset_ShopItem.h"

void UMVVM_ShopItem::InitFromDataAsset(const UDataAsset_ShopItem* InShopItem, UTexture2D* LoadedIcon)
{
	if (!InShopItem) return;

	ShopItemData = InShopItem;
	RawPrice = InShopItem->GetPrice();

	SetIcon(LoadedIcon);
	SetItemName(InShopItem->GetItemName());
	SetItemDescription(InShopItem->GetItemDescription());

	SetPriceText(FText::Format(FText::FromString(TEXT("{0}G")),FText::AsNumber(FMath::RoundToInt(RawPrice))));
	SetbCanAfford(false);
}

void UMVVM_ShopItem::UpdateAffordability(float CurrentGold)
{
	SetbCanAfford(CurrentGold >= RawPrice);
}


void UMVVM_ShopItem::SetIcon(UTexture2D* NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(Icon, NewValue);
}

void UMVVM_ShopItem::SetItemName(const FText& NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(ItemName, NewValue);
}

void UMVVM_ShopItem::SetItemDescription(const FText& NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(ItemDescription, NewValue);
}

void UMVVM_ShopItem::SetPriceText(const FText& NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(PriceText, NewValue);
}

void UMVVM_ShopItem::SetbCanAfford(bool NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(bCanAfford, NewValue);
}
