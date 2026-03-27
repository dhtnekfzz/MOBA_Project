// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/MAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/MAbilitySystemStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UMAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter=Cast<ACharacter>(TryGetPawnOwner());
	if (!OwnerCharacter) return;
	
	OwnerMovementComponent=OwnerCharacter->GetCharacterMovement();

	UAbilitySystemComponent* OwnerASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC)
		OwnerASC->RegisterGameplayTagEvent(MGameplayTags::State_Aim).AddUObject(this, &UMAnimInstance::OwnerAimTagChanged);

}

void UMAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!OwnerCharacter) return;

	FVector Velocity=OwnerCharacter->GetVelocity();
	Speed=Velocity.Length();
	FRotator BodyRot=OwnerCharacter->GetActorRotation();
	FRotator BodyRotDelta=UKismetMathLibrary::NormalizedDeltaRotator(BodyRot, BodyPrevRot);
	BodyPrevRot=BodyRot;

	YawSpeed=BodyRotDelta.Yaw/DeltaTime;
	float YawLerpSpeed=YawSpeedSmoothLerpSpeed;
	if (!YawSpeed) YawLerpSpeed=YawSpeedLerpToZeroSpeed;

	SmoothedYawSpeed=UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaTime, YawLerpSpeed);
	FRotator ControlRot=OwnerCharacter->GetBaseAimRotation();
	LookRotOffset=UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, BodyRot);

	FwdSpeed=Velocity.Dot(ControlRot.Vector());
	RightSpeed=-Velocity.Dot(ControlRot.Vector().Cross(FVector::UpVector));

	if (OwnerMovementComponent) bIsJumping=OwnerMovementComponent->IsFalling();
		
}

void UMAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

bool UMAnimInstance::ShouldDoFullBody() const
{
	return (GetSpeed()<=0) && !(GetIsAiming());
}

void UMAnimInstance::OwnerAimTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount !=0;
}
