// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/MAssetManager.h"
#include "DataAssets/DataAsset_ShopItem.h"
#include "DataAssets/DA_MCharacterDefinition.h"

UMAssetManager& UMAssetManager::Get()
{
	UMAssetManager* Singleton = Cast<UMAssetManager>(GEngine->AssetManager.Get());
	if (Singleton)
		return *Singleton;

	UE_LOG(LogLoad, Fatal,
		TEXT("UMShopAssetManager::Get() failed. "
		     "DefaultEngine.ini 에 다음 설정을 추가하세요:\n"
		     "[/Script/Engine.Engine]\n"
		     "AssetManagerClassName=/Script/MOBA_Project.MShopAssetManager"));
	
	return *NewObject<UMAssetManager>();
}

void UMAssetManager::LoadShopItems(const FStreamableDelegate& Callback)
{
	if (bShopItemsLoaded)
	{
		Callback.ExecuteIfBound();
		return;
	}
	
	LoadPrimaryAssetsWithType(UDataAsset_ShopItem::GetShopItemAssetType(),TArray<FName>(),FStreamableDelegate::CreateUObject(this,&UMAssetManager::OnShopItemsLoaded, Callback));
}

bool UMAssetManager::GetLoadedShopItems(TArray<const UDataAsset_ShopItem*>& OutItems) const
{
	TArray<UObject*> LoadedObjects;
	const bool bLoaded = GetPrimaryAssetObjectList(UDataAsset_ShopItem::GetShopItemAssetType(), LoadedObjects);

	if (bLoaded)
	{
		OutItems.Reserve(LoadedObjects.Num());
		for (UObject* Obj : LoadedObjects)
			if (UDataAsset_ShopItem* Item = Cast<UDataAsset_ShopItem>(Obj))
				OutItems.Add(Item);
	}

	return bLoaded && !OutItems.IsEmpty();
}


const FItemCollection* UMAssetManager::GetCombinationForItem(const UDataAsset_ShopItem* Item) const
{
	return CombinationMap.Find(Item);
}

const FItemCollection* UMAssetManager::GetIngredientForItem(const UDataAsset_ShopItem* Item) const
{
	return IngredientMap.Find(Item);
}

void UMAssetManager::OnShopItemsLoaded(FStreamableDelegate Callback)
{
	Callback.ExecuteIfBound();
	BuildItemMaps();
	bShopItemsLoaded = true;

}

void UMAssetManager::BuildItemMaps()
{
	CombinationMap.Empty();
	IngredientMap.Empty();

	TArray<const UDataAsset_ShopItem*> LoadedItems;
	if (!GetLoadedShopItems(LoadedItems)) return;

	for (const UDataAsset_ShopItem* Item : LoadedItems)
	{
		if (!Item) continue;

		const TArray<TSoftObjectPtr<UDataAsset_ShopItem>>& SoftIngredients =Item->GetIngredientItems();
		if (SoftIngredients.IsEmpty()) continue;

		TArray<const UDataAsset_ShopItem*> IngredientPtrs;
		IngredientPtrs.Reserve(SoftIngredients.Num());

		for (const TSoftObjectPtr<UDataAsset_ShopItem>& Soft : SoftIngredients)
		{
			const UDataAsset_ShopItem* Ingredient = Soft.Get();

			IngredientPtrs.Add(Ingredient);
			AddToCombinationMap(Ingredient, Item);
		}

		if (!IngredientPtrs.IsEmpty())
			IngredientMap.Add(Item, FItemCollection(IngredientPtrs));
	}
}

void UMAssetManager::LoadCharacterDefinitions(const FStreamableDelegate& Callback)
{
	LoadPrimaryAssetsWithType(UDA_MCharacterDefinition::GetCharacterDefinitionAssetType(),TArray<FName>(),Callback);
}

bool UMAssetManager::GetLoadedCharacterDefinitions(TArray<UDA_MCharacterDefinition*>& Out) const
{
	TArray<UObject*> LoadedObjects;
	const bool bLoaded = GetPrimaryAssetObjectList(UDA_MCharacterDefinition::GetCharacterDefinitionAssetType(), LoadedObjects);

	if (bLoaded)
	{
		for (UObject* Obj : LoadedObjects)
			if (UDA_MCharacterDefinition* Def = Cast<UDA_MCharacterDefinition>(Obj))
				Out.Add(Def);
	}
	
	return bLoaded && !Out.IsEmpty();
}

void UMAssetManager::AddToCombinationMap(const UDataAsset_ShopItem* Ingredient, const UDataAsset_ShopItem* CombinationItem)
{
	if (FItemCollection* Existing = CombinationMap.Find(Ingredient))
		Existing->AddItem(CombinationItem, true);
	else
		CombinationMap.Add(Ingredient,FItemCollection(TArray<const UDataAsset_ShopItem*>{ CombinationItem }));
}
