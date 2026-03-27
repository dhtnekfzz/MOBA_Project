// Fill out your copyright notice in the Description page of Project Settings.

#include "DataAssets/DA_MCharacterDefinition.h"
#include "Character/MPlayerCharacter.h"
#include "DataAssets/StartUpData/DataAsset_PlayerStartUpData.h"

FPrimaryAssetId UDA_MCharacterDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetCharacterDefinitionAssetType(), GetFName());
}

FPrimaryAssetType UDA_MCharacterDefinition::GetCharacterDefinitionAssetType()
{
	return FPrimaryAssetType("CharacterDefinition");
}

UTexture2D* UDA_MCharacterDefinition::LoadIcon() const
{
	CharacterIcon.LoadSynchronous();
	if (CharacterIcon.IsValid())
		return CharacterIcon.Get();

	return nullptr;
}

TSubclassOf<AMPlayerCharacter> UDA_MCharacterDefinition::LoadCharacterClass() const
{
	CharacterClass.LoadSynchronous();
	if (CharacterClass.IsValid())
		return CharacterClass.Get();
	
	return TSubclassOf<AMPlayerCharacter>();
}

USkeletalMesh* UDA_MCharacterDefinition::LoadDisplayMesh() const
{
	TSubclassOf<AMPlayerCharacter> LoadedClass = LoadCharacterClass();
	if (!LoadedClass) return nullptr;

	ACharacter* CDO = Cast<ACharacter>(LoadedClass.GetDefaultObject());
	if (!CDO) return nullptr;

	return CDO->GetMesh() ? CDO->GetMesh()->GetSkeletalMeshAsset() : nullptr;
}

TSubclassOf<UAnimInstance> UDA_MCharacterDefinition::LoadDisplayAnimBP() const
{
	DisplayAnimBP.LoadSynchronous();
	if (DisplayAnimBP.IsValid())
		return DisplayAnimBP.Get();

	return TSubclassOf<UAnimInstance>();
}

TArray<FAbilityWidgetData*> UDA_MCharacterDefinition::GetAbilityWidgetDataList() const
{
	if (!AbilityWidgetDataTable) return {};
	
	TArray<FAbilityWidgetData*> OutRowArray;
	AbilityWidgetDataTable->GetAllRows<FAbilityWidgetData>(TEXT(""),OutRowArray);
	
	return OutRowArray;
}

