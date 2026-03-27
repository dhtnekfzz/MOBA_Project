// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Styling/SlateBrush.h"
#include "MWidgetBlueprintLibrary.generated.h"


UCLASS()
class MOBA_PROJECT_API UMWidgetBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="MOBA|Widget|Conversion",meta=(BlueprintThreadSafe, DisplayName="Texture To Slate Brush"))
	static FSlateBrush TextureToSlateBrush(UTexture2D* Texture);

	UFUNCTION(BlueprintPure, Category="MOBA|Widget|Conversion",meta=(BlueprintThreadSafe, DisplayName="IntToVisibility"))
	static ESlateVisibility IntToVisibility(float Value);
};
