// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/MGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MGameplayTags.h"
#include "GameFramework/Character.h"
#include "GAS/Abilities/GAP_Launched.h"

bool UMGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{

	FGameplayAbilitySpec* AbilitySpec=ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	if (AbilitySpec &&  AbilitySpec->Level<=0) return false;

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

}

void UMGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy==EMAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
	}
}

void UMGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy==EMAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
	}
}

AActor* UMGameplayAbility::GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const
{
	AActor* AvatarActor=GetAvatarActorFromActorInfo();
	if (!AvatarActor) return nullptr;

	FVector Location;
	FRotator Rotation;
	AvatarActor->GetActorEyesViewPoint(Location, Rotation);

	FVector AimEnd=Location+Rotation.Vector()*AimDistance;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(AvatarActor);

	FCollisionObjectQueryParams CollisionObjectQueryParams;
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	if (bShouldDrawDebug)
		DrawDebugLine(GetWorld(), Location, AimEnd, FColor::Red, false, 2.f, 0U, 3.f);

	TArray<FHitResult> HitResults;
	if (GetWorld()->LineTraceMultiByObjectType(HitResults, Location, AimEnd, CollisionObjectQueryParams, CollisionQueryParams))
	{
		for (FHitResult& HitResult : HitResults)
		{
			if (IsActorTeamAttitudeIs(HitResult.GetActor(), TeamAttitude))
				return HitResult.GetActor();
		}
	}

	return nullptr;
}

UAnimInstance* UMGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMeshComp=GetOwningComponentFromActorInfo();
	
	if (!OwnerSkeletalMeshComp) return nullptr;
	return OwnerSkeletalMeshComp->GetAnimInstance();
	
}

void UMGameplayAbility::PushSelf(const FVector& PushVel)
{
	ACharacter* OwningAvatarCharacter=GetOwningAvatarCharacter();
	if (!OwningAvatarCharacter) return;

	OwningAvatarCharacter->LaunchCharacter(PushVel, true, true);
	
}

void UMGameplayAbility::PushTarget(AActor* Target, const FVector& PushVel)
{
	if (!Target) return;

	FGameplayEventData EventData;

	FGameplayAbilityTargetData_SingleTargetHit* HitData=new FGameplayAbilityTargetData_SingleTargetHit;
	FHitResult HitResult;
	HitResult.ImpactNormal=PushVel;
	HitData->HitResult=HitResult;
	EventData.TargetData.Add(HitData);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, MGameplayTags::Ability_Passive_Launch_Activate, EventData);
}

void UMGameplayAbility::PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel)
{
	for (AActor* Target : Targets)
		PushTarget(Target, PushVel);
}

void UMGameplayAbility::PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVel)
{
	TArray<AActor*> Targets=UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargets(Targets, PushVel);
}

void UMGameplayAbility::PushTargetsFromOwnerLocation(const TArray<AActor*>& Targets, float PushSpeed)
{
	AActor* OwningAvatarCharacter=GetOwningAvatarCharacter();
	if (!OwningAvatarCharacter) return;

	FVector OwnerAvatarLocation=OwningAvatarCharacter->GetActorLocation();
	PushTargetsFromLocation(Targets, OwnerAvatarLocation, PushSpeed);
}

void UMGameplayAbility::PushTargetsFromOwnerLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float PushSpeed)
{
	TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargetsFromOwnerLocation(TargetActors, PushSpeed);
}

void UMGameplayAbility::PushTargetsFromLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	const FVector& FromLocation, float PushSpeed)
{
	TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	PushTargetsFromLocation(Targets, FromLocation, PushSpeed);
}

void UMGameplayAbility::PushTargetsFromLocation(const TArray<AActor*>& Targets, const FVector& FromLocation, float PushSpeed)
{
	for (AActor* Target : Targets)
	{
		FVector PushDir = Target->GetActorLocation() - FromLocation;
		PushDir.Z = 0;
		PushDir.Normalize();

		PushTarget(Target, PushDir * PushSpeed);
	}
}

void UMGameplayAbility::PlayMontageLocally(UAnimMontage* MontageToPlay)
{
	UAnimInstance* OwnerAnimInst=GetOwnerAnimInstance();
	
	if (!OwnerAnimInst || OwnerAnimInst->Montage_IsPlaying(MontageToPlay)) return;
	OwnerAnimInst->Montage_Play(MontageToPlay);
}

bool UMGameplayAbility::IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const
{
	if (OtherActor)
	{
		IGenericTeamAgentInterface* OwnerTeamAgentInterface=Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
		if (OwnerTeamAgentInterface)
			return OwnerTeamAgentInterface->GetTeamAttitudeTowards(*OtherActor)==TeamAttitude;
	}

	return false;
}

ACharacter* UMGameplayAbility::GetOwningAvatarCharacter()
{
	if (!AvatarCharacter)
		AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	return AvatarCharacter;
	
}

void UMGameplayAbility::ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	FGameplayEffectSpecHandle EffectSpecHandle=MakeOutgoingGameplayEffectSpec(GameplayEffect, Level);

	FGameplayEffectContextHandle EffectContext=MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
	EffectContext.AddHitResult(HitResult);

	EffectSpecHandle.Data->SetContext(EffectContext);

	ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor()));
}

void UMGameplayAbility::SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	UAbilitySystemComponent* OwnerASC=GetAbilitySystemComponentFromActorInfo();
	
	if (OwnerASC)
		OwnerASC->HandleGameplayEvent(EventTag, &EventData);
}

FGenericTeamId UMGameplayAbility::GetOwnerTeamId() const
{
	IGenericTeamAgentInterface* OwnerTeamInterface=Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());

	if (!OwnerTeamInterface) return FGenericTeamId::NoTeam;
	return OwnerTeamInterface->GetGenericTeamId();
}
