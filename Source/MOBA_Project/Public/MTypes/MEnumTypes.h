// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "MEnumTypes.generated.h"

UENUM(BlueprintType)
enum class EMAbilityInputID : uint8
{
	None			UMETA(DisplayName = "None"),
	BasicAttack		UMETA(DisplayName="Basic Attack"),
	Aim				UMETA(DisplayName="Aim"),
	AbilityOne		UMETA(DisplayName="Ability One"),
	AbilityTwo		UMETA(DisplayName="Ability Two"),
	AbilityThree 	UMETA(DisplayName="Ability Three"),
	AbilityFour 	UMETA(DisplayName="Ability Four"),
	AbilityFive 	UMETA(DisplayName="Ability Five"),
	Confirm 		UMETA(DisplayName="Confirm"),
	Cancel 			UMETA(DisplayName="Cancel"),
};

UENUM(BlueprintType)
enum class EMAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};