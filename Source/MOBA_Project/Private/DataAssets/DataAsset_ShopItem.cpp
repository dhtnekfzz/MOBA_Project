// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/DataAsset_ShopItem.h"

#include "GAS/Abilities/MGameplayAbility.h"

FPrimaryAssetId UDataAsset_ShopItem::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetShopItemAssetType(), GetFName());
}

FPrimaryAssetType UDataAsset_ShopItem::GetShopItemAssetType()
{
	return FPrimaryAssetType("ShopItem");
}

UGameplayAbility* UDataAsset_ShopItem::GetGrantedAbilityCDO() const
{
	if (!GrantedAbility) return nullptr;
	return GrantedAbility->GetDefaultObject<UMGameplayAbility>();
}
