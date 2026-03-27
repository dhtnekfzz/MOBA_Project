// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MWidgetBlueprintLibrary.h"

#include "Components/SlateWrapperTypes.h"
#include "Engine/Texture2D.h"

FSlateBrush UMWidgetBlueprintLibrary::TextureToSlateBrush(UTexture2D* Texture)
{
	FSlateBrush Brush;

	if (Texture)
	{
		Brush.SetResourceObject(Texture);
		Brush.DrawAs = ESlateBrushDrawType::Image;
	}
	else
	{
		Brush.DrawAs=ESlateBrushDrawType::NoDrawType;
	}

	return Brush;
}

ESlateVisibility UMWidgetBlueprintLibrary::IntToVisibility(float Value)
{
	if (Value<=0)
	{
		return ESlateVisibility::Hidden;
	}

	return ESlateVisibility::Visible;
}
