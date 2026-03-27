// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "MInventoryItemDragDropOp.generated.h"

class UInventoryItemWidget;
class UItemWidget;


UCLASS()
class MOBA_PROJECT_API UMInventoryItemDragDropOp : public UDragDropOperation
{
	GENERATED_BODY()

public:
	void SetDraggedItem(UInventoryItemWidget* DraggedItem);

private:
	UPROPERTY(EditDefaultsOnly, Category="DragDrop")
	TSubclassOf<UItemWidget> DragVisualClass;
};
