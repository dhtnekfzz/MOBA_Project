// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MTypes/MStructTypes.h"
#include "InventoryContainerWidget.generated.h"

class UInventoryItem;
class UInventoryItemWidget;
class UInventoryContextMenuWidget;
class UInventoryComponent;
class UShopSubsystem;
class UWrapBox;


UCLASS()
class MOBA_PROJECT_API UInventoryContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath,const FWidgetPath& NewWidgetPath,const FFocusEvent& InFocusEvent) override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWrapBox> ItemList;

	UPROPERTY(EditDefaultsOnly, Category="Inventory",meta=(AllowPrivateAccess="true"))
	TSubclassOf<UInventoryItemWidget> ItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UInventoryContextMenuWidget> ContextMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UInventoryContextMenuWidget> ContextMenuWidget;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> CachedInvComp;

	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemWidget>> SlotWidgets;

	UPROPERTY()
	TMap<FMInventoryItemHandle, UInventoryItemWidget*> HandleToWidgetMap;
	
	void Initialize(UShopSubsystem* ShopSub, APawn* OwnerPawn);
	void InitSlotWidgets(UShopSubsystem* ShopSub, int32 Capacity);
	void SpawnContextMenu();
	
	UFUNCTION()
	void OnPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void OnItemAdded(const FMInventoryItemHandle& Handle, int32 InSlot);
	void OnItemRemoved(const FMInventoryItemHandle& Handle);
	void OnItemStackCountChanged(const FMInventoryItemHandle& Handle, int32 NewCount);
	
	void HandleItemDragDrop(UInventoryItemWidget* DestWidget, UInventoryItemWidget* SourceWidget);

	UInventoryItemWidget* GetNextAvailableSlot() const;

	void SetContextMenuVisible(bool bVisible);
	void ToggleContextMenu(const FMInventoryItemHandle& ItemHandle);
	void ClearContextMenu();

	UFUNCTION()
	void SellFocusedItem();

	UFUNCTION()
	void UseFocusedItem();

	UPROPERTY()
	TObjectPtr<UShopSubsystem> CachedShopSub;
	
	FMInventoryItemHandle FocusedItemHandle;
};
