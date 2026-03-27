// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MTypes/MStructTypes.h"
#include "MAssetManager.generated.h"

class UDataAsset_ShopItem;
class UDA_MCharacterDefinition;


UCLASS()
class MOBA_PROJECT_API UMAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UMAssetManager& Get();
	
	void LoadShopItems(const FStreamableDelegate& Callback);
	bool GetLoadedShopItems(TArray<const UDataAsset_ShopItem*>& OutItems) const;
	bool AreShopItemsLoaded() const { return bShopItemsLoaded; }
	void LoadCharacterDefinitions(const FStreamableDelegate& Callback);
	bool GetLoadedCharacterDefinitions(TArray<UDA_MCharacterDefinition*>& Out) const;
	
	const FItemCollection* GetCombinationForItem(const UDataAsset_ShopItem* Item) const;
	const FItemCollection* GetIngredientForItem(const UDataAsset_ShopItem* Item) const;

private:
	void OnShopItemsLoaded(FStreamableDelegate Callback);
	void BuildItemMaps();
	void AddToCombinationMap(const UDataAsset_ShopItem* Ingredient, const UDataAsset_ShopItem* CombinationItem);
	
	UPROPERTY()
	TMap<const UDataAsset_ShopItem*, FItemCollection> CombinationMap;

	UPROPERTY()
	TMap<const UDataAsset_ShopItem*, FItemCollection> IngredientMap;

	bool bShopItemsLoaded = false;
};
