// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ItemWidget.h"

#include "IDetailTreeNode.h"
#include "Components/Image.h"
#include "View/MVVMView.h"
#include "Widgets/MVVM/MVVM_ShopItem.h"
#include "Widgets/Subsystems/ShopSubsystem.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
}



UUserWidget* UItemWidget::SetToolTipWidget(const UDataAsset_ShopItem* Item)
{
	if (Item)
	{
		if (GetOwningPlayer() || ItemToolTipClass)
		{
			UUserWidget* ToolTip=CreateWidget(GetOwningPlayer(), ItemToolTipClass);
			
			UMVVM_ShopItem* VM=GetOwningLocalPlayer()->GetSubsystem<UShopSubsystem>()->GetViewModelForItem(Item);
			
			if (VM)
			{
				if (UMVVMView* View = ToolTip->GetExtension<UMVVMView>())
					View->SetViewModel(FName("ShopVM"), VM);
			}

			SetToolTip(ToolTip);
			return ToolTip;
		}
	}
	else
	{
		SetToolTip(nullptr);
	}

	return nullptr;
}


FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		return FReply::Handled().SetUserFocus(TakeWidget());
	}

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		TSharedRef<SWidget> SlateWidget = TakeWidget();
		return FReply::Handled().SetUserFocus(SlateWidget).DetectDrag(SlateWidget, EKeys::LeftMouseButton);
	}

	return SuperReply;
}

FReply UItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (HasAnyUserFocus())
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			RightButtonClicked();
			return FReply::Handled();
		}
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			LeftButtonClicked();
			return FReply::Handled();
		}
	}

	return SuperReply;
}

void UItemWidget::RightButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Right mouse button clicked"));
}

void UItemWidget::LeftButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Left mouse button clicked"));
}
