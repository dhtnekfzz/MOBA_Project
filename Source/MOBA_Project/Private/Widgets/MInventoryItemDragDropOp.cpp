// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MInventoryItemDragDropOp.h"
#include "Widgets/MVVM/MVVM_InventorySlot.h"
#include "View/MVVMView.h"
#include "Widgets/InventoryItemWidget.h"  
#include "Widgets/ItemWidget.h"


// =============================================================================
void UMInventoryItemDragDropOp::SetDraggedItem(UInventoryItemWidget* DraggedItem)
{
	if (!DraggedItem) return;

	Payload = DraggedItem;

	if (DragVisualClass)
	{

		UItemWidget* Visual = CreateWidget<UItemWidget>(GetWorld(), DragVisualClass);
		if (Visual)
		{
			if (UMVVMView* View = Visual->GetExtension<UMVVMView>())
			{
				View->SetViewModel(FName("InventoryVM"), DraggedItem->GetViewModel());
			}
			
			DefaultDragVisual = Visual;
		}
	}
}
