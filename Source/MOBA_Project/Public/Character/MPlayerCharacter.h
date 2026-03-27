// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MCharacter.h"
#include "MPlayerCharacter.generated.h"

class UInventoryComponent;
struct FGameplayTag;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API AMPlayerCharacter : public AMCharacter
{
	GENERATED_BODY()

public:
	AMPlayerCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

#pragma region Component
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category="View")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category="View")
	class UCameraComponent* ViewCam;

	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent;

#pragma endregion

private:
	FVector GetLookRightDir() const;
	FVector GetLookFwdDir() const;
	FVector GetMoveFwdDir() const;

#pragma region Gameplay Ability
	
private:
	virtual void OnAimStateChanged(bool bIsAiming) override;
	
	UPROPERTY()
	class UMHeroAttributeSet* HeroAttributeSet;
	
#pragma endregion
	
#pragma region Inputs

private:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input", meta = (AllowPrivateAccess = "true"))
	class UDataAsset_InputConfig* InputConfigDataAsset;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);

	void Input_AbilityInputPressed(const FInputActionValue& InputActionValue, FGameplayTag InInputTag);
	void Input_AbilityInputReleased(const FInputActionValue& InputActionValue, FGameplayTag InInputTag);

	void Input_ToggleShop();
	void Input_ToggleGameplayMenu();

	void Input_ShiftPressed();
	void Input_ShiftReleased();

	bool bIsLearn=false;
	void LearnAbilityInputPressed();
	void LearnAbilityInputReleased();

	void SetInputEnabledFromPlayerController(bool bEnabled);

#pragma endregion

#pragma region Stun

private:
	virtual void OnStun() override;
	virtual void OnRecoverFromStun() override;
	
#pragma endregion

#pragma region Death

private:
	virtual void OnDead() override;
	virtual void OnRespawn() override;
	
#pragma endregion

#pragma region Camera

private:
	UPROPERTY(EditDefaultsOnly, Category="View")
	FVector CameraAimLocalOffset;

	UPROPERTY(EditDefaultsOnly, Category="View")
	float CameraLerpSpeed=20.f;

	FTimerHandle CameraLerpTimerHandle;

	void LerpCameraToLocalOffsetLocation(const FVector& Goal);
	void TickCameraLocalOffsetLerp(FVector Goal);

#pragma endregion
	
};



