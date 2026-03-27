// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ITreeNodeInterface.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Widgets/ItemWidget.h"
#include "ShopItemWidget.generated.h"

class UDataAsset_ShopItem;
class UShopItemWidget;
class UMVVM_ShopItem;
class UListView;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssued, const UDataAsset_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected,  const UShopItemWidget*);

UCLASS()
class MOBA_PROJECT_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry, public ITreeNodeInterface
{
	GENERATED_BODY()

public:
	FOnItemPurchaseIssued OnItemPurchaseIssued;
	FOnShopItemSelected   OnShopItemSelected;

	virtual UUserWidget* GetWidget() const override;
	virtual TArray<const ITreeNodeInterface*> GetInputs() const override;
	virtual TArray<const ITreeNodeInterface*> GetOuputs() const override;
	virtual const UObject* GetItemObject() const override;
	
	const UDataAsset_ShopItem* GetShopItemData() const;
	FORCEINLINE UMVVM_ShopItem* GetViewModel() const { return CachedVM; }

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void CopyFromOther(const UShopItemWidget* Source);

	UFUNCTION(BlueprintImplementableEvent, Category="Shop|MVVM")
	void OnViewModelSet(UMVVM_ShopItem* ViewModel);
	
	UFUNCTION(BlueprintCallable, Category="Shop")
	void PurchaseButtonClicked();

	UFUNCTION(BlueprintCallable, Category="Shop")
	void SelectButtonClicked();

protected:
	UPROPERTY()
	TObjectPtr<UMVVM_ShopItem> CachedVM;

	UPROPERTY()
	TObjectPtr<const UListView> ParentListView;
	
	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked()  override;

private:
	TArray<const ITreeNodeInterface*> ItemsToInterfaces(const TArray<const UDataAsset_ShopItem*>& Items) const;

	void InitWithViewModel(UMVVM_ShopItem* InVM);
};
