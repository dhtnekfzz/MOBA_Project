// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_ShopItem.generated.h"

class UDataAsset_ShopItem;

UCLASS()
class MOBA_PROJECT_API UMVVM_ShopItem : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitFromDataAsset(const UDataAsset_ShopItem* InShopItem, UTexture2D* LoadedIcon);
	
	void UpdateAffordability(float CurrentGold);
	
	FORCEINLINE const UDataAsset_ShopItem* GetShopItemData() const { return ShopItemData; }

	UTexture2D*  GetIcon() const { return Icon; }
	FText GetItemName() const { return ItemName; }
	FText GetItemDescription() const { return ItemDescription; }
	FText GetPriceText() const { return PriceText; }
	bool GetbCanAfford() const { return bCanAfford; }

	void SetIcon(UTexture2D* NewValue);
	void SetItemName(const FText& NewValue);
	void SetItemDescription(const FText& NewValue);
	void SetPriceText(const FText& NewValue);
	void SetbCanAfford(bool NewValue);

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FText ItemName;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FText ItemDescription;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FText PriceText;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	bool bCanAfford = false;

	UPROPERTY()
	TObjectPtr<const UDataAsset_ShopItem> ShopItemData;

	float RawPrice = 0.f;
};
