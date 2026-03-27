// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ShopItemTreeNode.h"

#include "DataAssets/DataAsset_ShopItem.h"
#include "Framework/MAssetManager.h"
#include "Widgets/MVVM/MVVM_ShopItem.h"
#include "Widgets/ShopItemWidget.h"
#include "Widgets/Subsystems/ShopSubsystem.h"
#include "View/MVVMView.h"


UShopItemTreeNode* UShopItemTreeNode::Create(
	UObject*                     Outer,
	const UDataAsset_ShopItem*   InItem,
	UShopSubsystem*              InSubsystem,
	TSubclassOf<UShopItemWidget> InWidgetClass,
	APlayerController*           InPC)
{
	if (!InItem || !InSubsystem || !InWidgetClass || !InPC)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopItemTreeNode::Create - null parameter"));
		return nullptr;
	}

	UShopItemTreeNode* Node = NewObject<UShopItemTreeNode>(Outer);
	Node->ShopItemData = InItem;
	Node->ViewModel = InSubsystem->GetViewModelForItem(InItem);
	Node->WidgetClass = InWidgetClass;
	Node->OwningPC = InPC;
	Node->CachedSubsystem  = InSubsystem;
	return Node;
}

UUserWidget* UShopItemTreeNode::GetWidget() const
{
	if (!OwningPC.IsValid() || !WidgetClass) return nullptr;

	UShopItemWidget* Widget = CreateWidget<UShopItemWidget>(OwningPC.Get(), WidgetClass);
	if (!Widget) return nullptr;

	if (ViewModel)
	{
		if (UMVVMView* MVVMView = Widget->GetExtension<UMVVMView>())
			MVVMView->SetViewModel(FName("ShopVM"), ViewModel);

		Widget->OnViewModelSet(ViewModel);
	}

	return Widget;
}


TArray<const ITreeNodeInterface*> UShopItemTreeNode::GetInputs() const
{
	if (!CachedInputNodes.IsEmpty())
	{
		// 이미 캐싱됨 → 바로 반환
		TArray<const ITreeNodeInterface*> Result;
		Result.Reserve(CachedInputNodes.Num());
		for (const TObjectPtr<UShopItemTreeNode>& Child : CachedInputNodes)
			if (Child) Result.Add(Child.Get());
		return Result;
	}

	if (!CachedSubsystem.IsValid()) return {};

	const FItemCollection* Collection =
		UMAssetManager::Get().GetCombinationForItem(ShopItemData.Get());

	return ItemsToNodes(Collection, CachedInputNodes);
}

TArray<const ITreeNodeInterface*> UShopItemTreeNode::GetOuputs() const
{
	if (!CachedOutputNodes.IsEmpty())
	{
		TArray<const ITreeNodeInterface*> Result;
		Result.Reserve(CachedOutputNodes.Num());
		for (const TObjectPtr<UShopItemTreeNode>& Child : CachedOutputNodes)
			if (Child) Result.Add(Child.Get());
		return Result;
	}

	if (!CachedSubsystem.IsValid()) return {};

	const FItemCollection* Collection = UMAssetManager::Get().GetIngredientForItem(ShopItemData.Get());

	return ItemsToNodes(Collection, CachedOutputNodes);
}


const UObject* UShopItemTreeNode::GetItemObject() const
{
	return ShopItemData.Get();
}

TArray<const ITreeNodeInterface*> UShopItemTreeNode::ItemsToNodes(const FItemCollection* Items, TArray<TObjectPtr<UShopItemTreeNode>>& OutCachedChildren) const
{
	if (!Items || !CachedSubsystem.IsValid())
		return {};

	UShopSubsystem* Subsystem  = CachedSubsystem.Get();
	APlayerController* PC      = OwningPC.Get();
	if (!Subsystem || !PC) return {};

	TArray<const UDataAsset_ShopItem*> ItemArray(Items->GetItems());

	TArray<const ITreeNodeInterface*> Result;
	Result.Reserve(ItemArray.Num());
	OutCachedChildren.Reserve(ItemArray.Num());

	for (const UDataAsset_ShopItem* Item : ItemArray)
	{
		if (!Item) continue;

		UShopItemTreeNode* ChildNode = UShopItemTreeNode::Create(
			const_cast<UShopItemTreeNode*>(this), 
			Item,
			Subsystem,
			WidgetClass,
			PC);

		if (ChildNode)
		{
			OutCachedChildren.Add(ChildNode); 
			Result.Add(ChildNode);
		}
	}

	return Result;
}
