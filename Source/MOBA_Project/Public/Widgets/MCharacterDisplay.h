// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MCharacterDisplay.generated.h"

class UDA_MCharacterDefinition;

UCLASS()
class MOBA_PROJECT_API AMCharacterDisplay : public AActor
{
	GENERATED_BODY()

public:
	AMCharacterDisplay();

	void ConfigureWithDefinition(const UDA_MCharacterDefinition* Definition);

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Display")
	class USkeletalMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category="Display")
	class UCameraComponent* ViewCamera;
};
