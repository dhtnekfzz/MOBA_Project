// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailTreeNode.h"
#include "MTypes/MStructTypes.h"
#include "Widgets/ItemWidget.h"
#include "InventoryItemWidget.generated.h"

class UInventoryItem;
class UMVVM_InventorySlot;
class UMInventoryItemDragDropOp;
class UInventoryItemWidget;


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemDropped,UInventoryItemWidget* /*DestinationWidget*/,UInventoryItemWidget* /*SourceWidget*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventoryButtonClicked,const FMInventoryItemHandle& /*Handle*/);


UCLASS()
class MOBA_PROJECT_API UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()

public:
	FOnInventoryItemDropped   OnInventoryItemDropped;
	FOnInventoryButtonClicked OnRightButtonClicked;
	FOnInventoryButtonClicked OnLeftButtonClicked;
	
	void SetupSlot(int32 InSlotIndex, UMVVM_InventorySlot* InVM);
	void UpdateInventoryItem(UInventoryItem* Item);
	void EmptySlot();


	UTexture2D* GetIconTexture() const;
	
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE FMInventoryItemHandle GetHandle() const { return Handle; }
	FORCEINLINE bool IsEmpty() const { return !Handle.IsValid(); }
	FORCEINLINE UMVVM_InventorySlot* GetViewModel() const { return InventoryViewModel; }
	FORCEINLINE UInventoryItem* GetInventoryItem() const { return CachedItem; }

	virtual void SetIcon(UTexture2D* IconTexture) override;

protected:
	virtual void NativeConstruct() override;

private:
	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;
	
	virtual void NativeOnDragDetected(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent,UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry,const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation) override;

	UPROPERTY(EditDefaultsOnly, Category="DragDrop", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UMInventoryItemDragDropOp> DragDropOpClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UTexture2D> EmptyTexture;

	UPROPERTY()
	TObjectPtr<UMVVM_InventorySlot> InventoryViewModel;

	UPROPERTY()
	TObjectPtr<UInventoryItem> CachedItem;
	
	FMInventoryItemHandle Handle;
	int32 SlotIndex = 0;
};
