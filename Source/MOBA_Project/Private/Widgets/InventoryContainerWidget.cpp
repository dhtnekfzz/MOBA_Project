// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryContainerWidget.h"
#include "Widgets/InventoryItemWidget.h"
#include "Widgets/InventoryContextMenuWidget.h"
#include "Widgets/Subsystems/ShopSubsystem.h"
#include "Widgets/MVVM/MVVM_InventorySlot.h"
#include "InventoryItem/InventoryComponent.h"
#include "InventoryItem/InventoryItem.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"


void UInventoryContainerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ULocalPlayer* LP = GetOwningLocalPlayer();
	if (!LP) return;

	UShopSubsystem* ShopSub = LP->GetSubsystem<UShopSubsystem>();
	if (!ShopSub) return;
	
	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!OwnerPawn)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			PC->OnPossessedPawnChanged.AddUniqueDynamic(this, &UInventoryContainerWidget::OnPawnChanged);
		}
		return;
	}
	Initialize(ShopSub, OwnerPawn);
}

void UInventoryContainerWidget::OnPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (!NewPawn) return;

	if (APlayerController* PC = GetOwningPlayer())
		PC->OnPossessedPawnChanged.RemoveDynamic(
			this, &UInventoryContainerWidget::OnPawnChanged);

	ULocalPlayer* LP = GetOwningLocalPlayer();
	if (!LP) return;

	UShopSubsystem* ShopSub = LP->GetSubsystem<UShopSubsystem>();
	if (!ShopSub) return;

	Initialize(ShopSub, NewPawn);
}

void UInventoryContainerWidget::Initialize(UShopSubsystem* ShopSub, APawn* OwnerPawn)
{
	CachedInvComp = OwnerPawn->GetComponentByClass<UInventoryComponent>();
	if (!CachedInvComp) return;
	
	const int32 Capacity = CachedInvComp->GetCapacity();
	InitSlotWidgets(ShopSub, Capacity);
	
	CachedShopSub=ShopSub;
	CachedShopSub->OnInventorySlotItemSet.AddUObject(this, &ThisClass::OnItemAdded);
	CachedShopSub->OnInventorySlotItemCleared.AddUObject(this, &ThisClass::OnItemRemoved);
	
	SpawnContextMenu();
}


void UInventoryContainerWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath,const FWidgetPath& NewWidgetPath,const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	if (ContextMenuWidget && !NewWidgetPath.ContainsWidget(ContextMenuWidget->GetCachedWidget().Get()))
	{
		ClearContextMenu();
	}
}


void UInventoryContainerWidget::InitSlotWidgets(UShopSubsystem* ShopSub, int32 Capacity)
{
	if (!ItemList || !ItemWidgetClass) return;
	
	ItemList->ClearChildren();
	SlotWidgets.Empty();
	HandleToWidgetMap.Empty();

	const TArray<TObjectPtr<UMVVM_InventorySlot>>& SlotVMs = ShopSub->GetSlotViewModels();

	for (int32 i = 0; i < Capacity; ++i)
	{
		UInventoryItemWidget* NewWidget = CreateWidget<UInventoryItemWidget>(this, ItemWidgetClass);
		if (!NewWidget) continue;

		UMVVM_InventorySlot* VM = SlotVMs.IsValidIndex(i) ? SlotVMs[i].Get() : nullptr;
		NewWidget->SetupSlot(i, VM);
		VM->UnbindAndClear();

		UWrapBoxSlot* BoxSlot = ItemList->AddChildToWrapBox(NewWidget);
		if (BoxSlot) BoxSlot->SetPadding(FMargin(2.f));

		SlotWidgets.Add(NewWidget);

		NewWidget->OnInventoryItemDropped.AddUObject(this, &UInventoryContainerWidget::HandleItemDragDrop);
		NewWidget->OnLeftButtonClicked.AddUObject(CachedInvComp, &UInventoryComponent::TryActivateItem);
		NewWidget->OnRightButtonClicked.AddUObject(this, &UInventoryContainerWidget::ToggleContextMenu);
	}
}

void UInventoryContainerWidget::SpawnContextMenu()
{
	if (!ContextMenuWidgetClass) return;
	ContextMenuWidget = CreateWidget<UInventoryContextMenuWidget>(this, ContextMenuWidgetClass);
	if (!ContextMenuWidget) return;

	ContextMenuWidget->GetSellButtonClickedEvent().AddDynamic(this, &UInventoryContainerWidget::SellFocusedItem);
	ContextMenuWidget->GetUseButtonClickedEvent().AddDynamic(this, &UInventoryContainerWidget::UseFocusedItem);

	ContextMenuWidget->AddToViewport(1);
	SetContextMenuVisible(false);
}


void UInventoryContainerWidget::OnItemAdded(const FMInventoryItemHandle& Handle, int32 InSlot)
{
	UInventoryItemWidget* SlotWidget = SlotWidgets[InSlot];
	if (!SlotWidget) return;

	UInventoryItem* Item=CachedInvComp->GetInventoryItemByHandle(Handle);
	SlotWidget->UpdateInventoryItem(Item);
	
	HandleToWidgetMap.Add(Handle, SlotWidget);
	CachedInvComp->ItemSlotChanged(Handle, SlotWidget->GetSlotIndex());
}


void UInventoryContainerWidget::OnItemRemoved(const FMInventoryItemHandle& Handle)
{
	if (UInventoryItemWidget** Found = HandleToWidgetMap.Find(Handle))
	{
		(*Found)->EmptySlot();
		HandleToWidgetMap.Remove(Handle);
		if (FocusedItemHandle == Handle) ClearContextMenu();
	}
}


void UInventoryContainerWidget::OnItemStackCountChanged(const FMInventoryItemHandle& Handle,int32 NewCount)
{

}


void UInventoryContainerWidget::HandleItemDragDrop(UInventoryItemWidget* DestWidget,UInventoryItemWidget* SourceWidget)
{
	if (!DestWidget || !SourceWidget || !CachedInvComp) return;
	
	UInventoryItem* SrcItem  = SourceWidget->GetInventoryItem();
	UInventoryItem* DestItem = DestWidget->GetInventoryItem();

	const int32 DestSlot   = DestWidget->GetSlotIndex();
	const int32 SourceSlot = SourceWidget->GetSlotIndex();

	DestWidget->SetupSlot(DestSlot, DestWidget->GetViewModel()); 
	SourceWidget->SetupSlot(SourceSlot, SourceWidget->GetViewModel());

	DestWidget->UpdateInventoryItem(SrcItem);   
	SourceWidget->UpdateInventoryItem(DestItem);

	HandleToWidgetMap.Add(SrcItem->GetHandle(), DestWidget);
	if (DestItem)
		HandleToWidgetMap.Add(DestItem->GetHandle(), SourceWidget);
	else
		HandleToWidgetMap.Remove(SourceWidget->GetHandle()); 
	
	CachedShopSub->UpdateHandleToVMMapping(SrcItem->GetHandle(), DestWidget->GetViewModel());
	if (DestItem)
		CachedShopSub->UpdateHandleToVMMapping(DestItem->GetHandle(), SourceWidget->GetViewModel());

	CachedInvComp->ItemSlotChanged(SrcItem->GetHandle(), DestSlot);
	if (DestItem)
		CachedInvComp->ItemSlotChanged(DestItem->GetHandle(), SourceSlot);
}


UInventoryItemWidget* UInventoryContainerWidget::GetNextAvailableSlot() const
{
	for (UInventoryItemWidget* Widget : SlotWidgets)
		if (Widget && Widget->IsEmpty())
			return Widget;
	return nullptr;
}


void UInventoryContainerWidget::SetContextMenuVisible(bool bVisible)
{
	if (ContextMenuWidget)
		ContextMenuWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}


void UInventoryContainerWidget::ToggleContextMenu(const FMInventoryItemHandle& ItemHandle)
{
	if (FocusedItemHandle == ItemHandle || !ContextMenuWidget)
	{
		ClearContextMenu();
		return;
	}

	FocusedItemHandle = ItemHandle;

	UInventoryItemWidget** FoundWidget = HandleToWidgetMap.Find(ItemHandle);
	if (!FoundWidget || !(*FoundWidget)) return;

	UInventoryItemWidget* ItemWidget = *FoundWidget;
	SetContextMenuVisible(true);

	FVector2D ItemAbsPos = ItemWidget->GetCachedGeometry().GetAbsolutePositionAtCoordinates(FVector2D(1.f, 0.5f));

	FVector2D PixelPos, ViewportPos;
	USlateBlueprintLibrary::AbsoluteToViewport(this, ItemAbsPos, PixelPos, ViewportPos);

	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		int32 ViewportX, ViewportY;
		PC->GetViewportSize(ViewportX, ViewportY);
		const float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		const float Overshoot =PixelPos.Y + ContextMenuWidget->GetDesiredSize().Y * Scale - ViewportY;
		if (Overshoot > 0.f)
			PixelPos.Y -= Overshoot;
	}

	ContextMenuWidget->SetPositionInViewport(PixelPos);
}

void UInventoryContainerWidget::ClearContextMenu()
{
	SetContextMenuVisible(false);
	FocusedItemHandle = FMInventoryItemHandle::InvalidHandle();
}


void UInventoryContainerWidget::SellFocusedItem()
{
	if (CachedInvComp && FocusedItemHandle.IsValid())
		CachedInvComp->SellItem(FocusedItemHandle);
	ClearContextMenu();
}

void UInventoryContainerWidget::UseFocusedItem()
{
	if (CachedInvComp && FocusedItemHandle.IsValid())
		CachedInvComp->TryActivateItem(FocusedItemHandle);
	ClearContextMenu();
}
