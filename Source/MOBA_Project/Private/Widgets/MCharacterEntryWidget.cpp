// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MCharacterEntryWidget.h"
#include "DataAssets/DA_MCharacterDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UMCharacterEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CharacterDefinition = Cast<UDA_MCharacterDefinition>(ListItemObject);
	if (!CharacterDefinition) return;

	UMaterialInstanceDynamic* DynMat = CharacterIcon->GetDynamicMaterial();
	if (DynMat)
	{
		DynMat->SetTextureParameterValue(IconParamName, CharacterDefinition->LoadIcon());
		DynMat->SetScalarParameterValue(SaturationParamName, 1.f);
	}

	CharacterNameText->SetText(FText::FromString(CharacterDefinition->GetCharacterDisplayName()));
}

void UMCharacterEntryWidget::SetSelected(bool bIsSelected)
{
	if (!CharacterIcon) return;

	UMaterialInstanceDynamic* DynMat = CharacterIcon->GetDynamicMaterial();
	if (DynMat)
		DynMat->SetScalarParameterValue(SaturationParamName, bIsSelected ? 0.f : 1.f);
}
