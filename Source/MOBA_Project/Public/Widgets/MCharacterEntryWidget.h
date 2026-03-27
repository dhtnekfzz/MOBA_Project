// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MCharacterEntryWidget.generated.h"

class UDA_MCharacterDefinition;


UCLASS()
class MOBA_PROJECT_API UMCharacterEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	void SetSelected(bool bIsSelected);

	FORCEINLINE const UDA_MCharacterDefinition* GetCharacterDefinition() const{ return CharacterDefinition; }

private:
	UPROPERTY(meta=(BindWidget))
	class UImage* CharacterIcon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CharacterNameText;

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName IconParamName = "Icon";

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName SaturationParamName = "Saturation";

	UPROPERTY()
	TObjectPtr<const UDA_MCharacterDefinition> CharacterDefinition;
};
