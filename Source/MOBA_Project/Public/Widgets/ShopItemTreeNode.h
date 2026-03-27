// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MTypes/MStructTypes.h"
#include "UObject/Object.h"
#include "Widgets/ITreeNodeInterface.h"
#include "ShopItemTreeNode.generated.h"

class UDataAsset_ShopItem;
class UMVVM_ShopItem;
class UShopSubsystem;
class UShopItemWidget;


UCLASS()
class MOBA_PROJECT_API UShopItemTreeNode : public UObject, public ITreeNodeInterface
{
	GENERATED_BODY()

public:
	static UShopItemTreeNode* Create(
		UObject*                         Outer,
		const UDataAsset_ShopItem*       InItem,
		UShopSubsystem*                  InSubsystem,
		TSubclassOf<UShopItemWidget>     InWidgetClass,
		APlayerController*               InPC);
	
	virtual UUserWidget* GetWidget() const override;
	virtual TArray<const ITreeNodeInterface*> GetInputs() const override;
	virtual TArray<const ITreeNodeInterface*> GetOuputs() const override;
	
	virtual const UObject* GetItemObject() const override;

private:
	TArray<const ITreeNodeInterface*> ItemsToNodes(const FItemCollection* Items, TArray<TObjectPtr<UShopItemTreeNode>>& OutCachedChildren) const;
	
	UPROPERTY()
	TObjectPtr<const UDataAsset_ShopItem> ShopItemData;

	UPROPERTY()
	TObjectPtr<UMVVM_ShopItem> ViewModel;

	UPROPERTY()
	TSubclassOf<UShopItemWidget> WidgetClass;

	TWeakObjectPtr<APlayerController> OwningPC;
	TWeakObjectPtr<UShopSubsystem> CachedSubsystem;

	UPROPERTY()
	mutable TArray<TObjectPtr<UShopItemTreeNode>> CachedInputNodes;

	UPROPERTY()
	mutable TArray<TObjectPtr<UShopItemTreeNode>> CachedOutputNodes;
};
