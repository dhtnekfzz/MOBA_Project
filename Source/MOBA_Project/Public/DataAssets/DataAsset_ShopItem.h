// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_ShopItem.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UCLASS()
class MOBA_PROJECT_API UDataAsset_ShopItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetShopItemAssetType();

	FORCEINLINE UTexture2D* GetIcon() const { return Icon.LoadSynchronous(); }
	FORCEINLINE FText GetItemName() const { return ItemName; }
	FORCEINLINE FText GetItemDescription() const { return ItemDescription; }
	FORCEINLINE float GetPrice() const { return Price; }
	FORCEINLINE float GetSellPrice() const { return Price / 2.f; }
	FORCEINLINE bool IsConsumable() const { return bIsConsumable; }
	FORCEINLINE bool IsStackable() const { return bIsStackable; }
	FORCEINLINE int32  GetMaxStackCount() const { return MaxStackCount; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetEquippedEffect()   const { return EquippedEffect; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetConsumedEffect()   const { return ConsumedEffect; }
	FORCEINLINE TSubclassOf<UGameplayAbility> GetGrantedAbility()   const { return GrantedAbility; }
	FORCEINLINE const TArray<TSoftObjectPtr<UDataAsset_ShopItem>>& GetIngredientItems() const{ return IngredientItems;}
	
	UGameplayAbility* GetGrantedAbilityCDO() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	float Price = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayEffect> EquippedEffect;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayEffect> ConsumedEffect;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayAbility> GrantedAbility;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	bool bIsConsumable = false;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	bool bIsStackable = false;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	int32 MaxStackCount = 5;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TArray<TSoftObjectPtr<UDataAsset_ShopItem>> IngredientItems;
};
