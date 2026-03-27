// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MTypes/MStructTypes.h"
#include "DA_MCharacterDefinition.generated.h"

class UDataAsset_PlayerStartUpData;
class AMPlayerCharacter;


UCLASS(BlueprintType)
class MOBA_PROJECT_API UDA_MCharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetCharacterDefinitionAssetType();

	FString GetCharacterDisplayName() const { return CharacterName; }

	UTexture2D* LoadIcon() const;
	TSubclassOf<AMPlayerCharacter> LoadCharacterClass() const;
	USkeletalMesh* LoadDisplayMesh() const;
	TSubclassOf<UAnimInstance> LoadDisplayAnimBP() const;
	TArray<FAbilityWidgetData*> GetAbilityWidgetDataList() const;

private:
	UPROPERTY(EditDefaultsOnly, Category="Character")
	FString CharacterName;

	UPROPERTY(EditDefaultsOnly, Category="Character")
	TSoftObjectPtr<UTexture2D> CharacterIcon;

	UPROPERTY(EditDefaultsOnly, Category="Character")
	TSoftClassPtr<AMPlayerCharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly, Category="Character")
	TSoftClassPtr<UAnimInstance> DisplayAnimBP;
	
	UPROPERTY(EditDefaultsOnly, Category="Ability", meta=(RowType="AbilityWidgetData"))
	TObjectPtr<UDataTable> AbilityWidgetDataTable;;
};
