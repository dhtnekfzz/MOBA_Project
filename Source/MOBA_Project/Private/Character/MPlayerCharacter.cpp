// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "MGameplayTags.h"
#include "MOBA_Project.h"
#include "Camera/CameraComponent.h"
#include "Components/Input/MInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/MAbilitySystemComponent.h"
#include "GAS/MHeroAttributeSet.h"
#include "Controllers/MPlayerController.h"
#include "InventoryItem/InventoryComponent.h"
#include "Widgets/Subsystems/WidgetManagerSubsystem.h"

AMPlayerCharacter::AMPlayerCharacter()
{
	CameraBoom=CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation=true;
	CameraBoom->ProbeChannel=ECC_SpringArm;

	ViewCam=CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	InventoryComponent=CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");

	bUseControllerRotationYaw=false;
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->RotationRate=FRotator(0.0f,720.f,0.f);

	HeroAttributeSet=CreateDefaultSubobject<UMHeroAttributeSet>("Hero AttributeSet");
	
}

void AMPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UMInputComponent* MInputComponent = CastChecked<UMInputComponent>(PlayerInputComponent);

	ULocalPlayer* LocalPlayer=GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem=LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::Jump);
	MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_Ctrl, ETriggerEvent::Started, this, &ThisClass::LearnAbilityInputPressed);
	MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_Ctrl, ETriggerEvent::Completed, this, &ThisClass::LearnAbilityInputReleased);

	MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_Shop, ETriggerEvent::Started, this, &ThisClass::Input_ToggleShop);
	MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_GameplayMenu, ETriggerEvent::Started, this,	&ThisClass::Input_ToggleGameplayMenu);
	
	MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_Shift, ETriggerEvent::Started, this, &ThisClass::Input_ShiftPressed);
	MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_Shift, ETriggerEvent::Completed, this, &ThisClass::Input_ShiftReleased);

	// MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_Confirm, ETriggerEvent::Started, this, &ThisClass::Input_AbilityInputPressed);
	// MInputComponent->BindNativeInputAction(InputConfigDataAsset, MGameplayTags::InputTag_Cancel, ETriggerEvent::Completed, this, &ThisClass::Input_AbilityInputReleased);

	MInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
}

FVector AMPlayerCharacter::GetLookRightDir() const
{
	return ViewCam->GetRightVector();
}

FVector AMPlayerCharacter::GetLookFwdDir() const
{
	return ViewCam->GetForwardVector();
}

FVector AMPlayerCharacter::GetMoveFwdDir() const
{
	return FVector::CrossProduct(GetLookRightDir(), FVector::UpVector);
}

void AMPlayerCharacter::OnAimStateChanged(bool bIsAiming)
{
	LerpCameraToLocalOffsetLocation(bIsAiming ? CameraAimLocalOffset : FVector::ZeroVector);
}

void AMPlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector2d InputVal=InputActionValue.Get<FVector2D>();
	InputVal.Normalize();

	AddMovementInput(GetMoveFwdDir()*InputVal.Y+GetLookRightDir()*InputVal.X);
	
}

void AMPlayerCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	FVector2d InputVal=InputActionValue.Get<FVector2D>();

	AddControllerPitchInput(-InputVal.Y);
	AddControllerYawInput(InputVal.X);
}

void AMPlayerCharacter::Input_AbilityInputPressed(const FInputActionValue& InputActionValue, FGameplayTag InInputTag)
{
	bool bPressed=InputActionValue.Get<bool>();

	if (bPressed && bIsLearn)
	{
		UpgradeAbilityWithInputTag(InInputTag);
		return;
	}
	
	if (bPressed)
	{
		if (InInputTag==MGameplayTags::Ability_BasicAttack && ASC->HasMatchingGameplayTag(MGameplayTags::State_Aim))
		{
			ASC->OnAbilityInputPressed(MGameplayTags::InputTag_Confirm);
		}
		else
			ASC->OnAbilityInputPressed(InInputTag);
	}
	
	if (InInputTag==MGameplayTags::Ability_BasicAttack)
	{
		FGameplayTag BasicAttackTag=bPressed ? MGameplayTags::Ability_BasicAttack_Pressed : MGameplayTags::Ability_BasicAttack_Released;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, BasicAttackTag, FGameplayEventData());
		Server_SendGameplayEventToSelf(BasicAttackTag, FGameplayEventData());
	}
	
}

void AMPlayerCharacter::Input_AbilityInputReleased(const FInputActionValue& InputActionValue, FGameplayTag InInputTag)
{
	ASC->OnAbilityInputReleased(InInputTag);
}

void AMPlayerCharacter::Input_ToggleShop()
{
	if (AMPlayerController* PC = GetController<AMPlayerController>())
		PC->ToggleShopWidget();
}

void AMPlayerCharacter::Input_ToggleGameplayMenu()
{
	if (AMPlayerController* PC = GetController<AMPlayerController>())
		PC->ToggleGameplayMenuWidget();
}

void AMPlayerCharacter::Input_ShiftPressed()
{
	if (AMPlayerController* PC = GetController<AMPlayerController>())
		PC->EnableMouseMode();
}

void AMPlayerCharacter::Input_ShiftReleased()
{
	if (AMPlayerController* PC = GetController<AMPlayerController>())
		PC->DisableMouseMode();
}

void AMPlayerCharacter::LearnAbilityInputPressed()
{
	bIsLearn=true;
}

void AMPlayerCharacter::LearnAbilityInputReleased()
{
	bIsLearn=false;
}

void AMPlayerCharacter::SetInputEnabledFromPlayerController(bool bEnabled)
{
	APlayerController* PC=GetController<APlayerController>();

	if (!PC) return;

	if (bEnabled)
		EnableInput(PC);
	else
		DisableInput(PC);
}

void AMPlayerCharacter::OnStun()
{
	SetInputEnabledFromPlayerController(false);
}

void AMPlayerCharacter::OnRecoverFromStun()
{
	SetInputEnabledFromPlayerController(true);
}

void AMPlayerCharacter::OnDead()
{
	SetInputEnabledFromPlayerController(false);
}

void AMPlayerCharacter::OnRespawn()
{
	SetInputEnabledFromPlayerController(true);
}


void AMPlayerCharacter::LerpCameraToLocalOffsetLocation(const FVector& Goal)
{
	GetWorldTimerManager().ClearTimer(CameraLerpTimerHandle);
	CameraLerpTimerHandle=GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::TickCameraLocalOffsetLerp, Goal));
}

void AMPlayerCharacter::TickCameraLocalOffsetLerp(FVector Goal)
{
	FVector CurrentLocalOffset = ViewCam->GetRelativeLocation();
	if (FVector::Dist(CurrentLocalOffset, Goal) < 1.f)
	{
		ViewCam->SetRelativeLocation(Goal);
		return;
	}

	float LerpAlpha = FMath::Clamp(GetWorld()->GetDeltaSeconds() * CameraLerpSpeed, 0.f, 1.f);
	FVector NewLocalOffset = FMath::Lerp(CurrentLocalOffset, Goal, LerpAlpha);
	ViewCam->SetRelativeLocation(NewLocalOffset);

	CameraLerpTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::TickCameraLocalOffsetLerp, Goal));
}