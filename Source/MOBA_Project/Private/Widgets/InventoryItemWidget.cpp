// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryItemWidget.h"

#include "InventoryItem/InventoryItem.h"
#include "Widgets/MVVM/MVVM_InventorySlot.h"
#include "Widgets/MInventoryItemDragDropOp.h"
#include "View/MVVMView.h"

void UInventoryItemWidget::NativeConstruct()
{
	Super::NativeConstruct(); 
	EmptySlot();
}

void UInventoryItemWidget::SetupSlot(int32 InSlotIndex, UMVVM_InventorySlot* InVM)
{
	SlotIndex   = InSlotIndex;
	InventoryViewModel = InVM;
}


void UInventoryItemWidget::UpdateInventoryItem(UInventoryItem* Item)
{
	if (!Item || !Item->IsValid())
	{
		EmptySlot();
		return;
	}

	CachedItem = Item;
	Handle=Item->GetHandle();
	
	if (InventoryViewModel)
	{
		InventoryViewModel->BindToItem(Item);
		if (UMVVMView* View = GetExtension<UMVVMView>())
		{
			View->SetViewModel(FName("InventoryVM"), InventoryViewModel);
		}
	}

	SetToolTipWidget(Item->GetShopItem());
}

void UInventoryItemWidget::EmptySlot()
{
	CachedItem = nullptr;
	Handle = FMInventoryItemHandle::InvalidHandle();

	SetToolTipWidget(nullptr);
	
	if (InventoryViewModel)
		InventoryViewModel->UnbindAndClear();
}


UTexture2D* UInventoryItemWidget::GetIconTexture() const
{
	return InventoryViewModel ? InventoryViewModel->GetIcon() : nullptr;
}


void UInventoryItemWidget::SetIcon(UTexture2D* IconTexture)
{

}


void UInventoryItemWidget::RightButtonClicked()
{
	if (!IsEmpty())
		OnRightButtonClicked.Broadcast(Handle);
}

void UInventoryItemWidget::LeftButtonClicked()
{
	if (!IsEmpty())
		OnLeftButtonClicked.Broadcast(Handle);
}


void UInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent,UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (IsEmpty() || !DragDropOpClass) return;

	UMInventoryItemDragDropOp* DragOp = NewObject<UMInventoryItemDragDropOp>(this, DragDropOpClass);
	if (DragOp)
	{
		DragOp->SetDraggedItem(this);
		OutOperation = DragOp;
	}
}


bool UInventoryItemWidget::NativeOnDrop(const FGeometry& InGeometry,const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation)
{
	if (UInventoryItemWidget* SourceWidget = Cast<UInventoryItemWidget>(InOperation->Payload))
	{
		if (SourceWidget == this) return false;

		if (SourceWidget->IsEmpty()) return false;

		OnInventoryItemDropped.Broadcast(this, SourceWidget);
		return true;
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
