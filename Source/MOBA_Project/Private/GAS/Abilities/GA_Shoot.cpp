// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_Shoot.h"

#include "AbilitySystemComponent.h"
#include "MGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/MAbilitySystemStatics.h"
#include "GAS/ProjectileActor.h"

class UAbilityTask_WaitGameplayEvent;

UGA_Shoot::UGA_Shoot()
{
	ActivationOwnedTags.AddTag(MGameplayTags::State_Aim);
	ActivationOwnedTags.AddTag(MGameplayTags::State_Crosshair);
}

void UGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_WaitGameplayEvent* WaitStartShootingEvent=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MGameplayTags::Ability_BasicAttack_Pressed);
		WaitStartShootingEvent->EventReceived.AddDynamic(this, &UGA_Shoot::StartShooting);
		WaitStartShootingEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitStopShootingEvent=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MGameplayTags::Ability_BasicAttack_Released);
		WaitStopShootingEvent->EventReceived.AddDynamic(this, &UGA_Shoot::StopShooting);
		WaitStopShootingEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitShootProjectileEvent=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MGameplayTags::Ability_Shoot, nullptr, false, false);
		WaitShootProjectileEvent->EventReceived.AddDynamic(this, &UGA_Shoot::ShootProjectile);
		WaitShootProjectileEvent->ReadyForActivation();
	}
		
}

void UGA_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	K2_EndAbility();
}

void UGA_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AimTargetAbilitySystemComponent)
	{
		AimTargetAbilitySystemComponent->RegisterGameplayTagEvent(MGameplayTags::State_Dead).RemoveAll(this);
		AimTargetAbilitySystemComponent=nullptr;
	}
	
	SendLocalGameplayEvent(MGameplayTags::Target_Updated, FGameplayEventData());
	StopShooting(FGameplayEventData());
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Shoot::StartShooting(FGameplayEventData Payload)
{

	UAbilityTask_PlayMontageAndWait* PlayShootMontage=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootMontage);
	PlayShootMontage->ReadyForActivation();
	
	
	// PlayMontageLocally(ShootMontage);
	

	FindAimTarget();
	StartAimTargetCheckTimer();
}

void UGA_Shoot::StopShooting(FGameplayEventData Payload)
{
	if (ShootMontage)
	{
		
	}
}

void UGA_Shoot::ShootProjectile(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		AActor* AvatarActor=GetAvatarActorFromActorInfo();
		FActorSpawnParameters SpawnPrams;
		SpawnPrams.Owner=AvatarActor;
		SpawnPrams.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SocketLocation=AvatarActor->GetActorLocation();
		USkeletalMeshComponent* MeshComp=GetOwningComponentFromActorInfo();

		if (MeshComp)
		{
			TArray<FName> OutNames;
			UGameplayTagsManager::Get().SplitGameplayTagFName(Payload.EventTag, OutNames);
			if (OutNames.Num()!=0)
			{
				FName SocketName=OutNames.Last();
				SocketLocation=MeshComp->GetSocketLocation(SocketName);
			}
		}

		AProjectileActor* Projectile=GetWorld()->SpawnActor<AProjectileActor>(ProjectileClass, SocketLocation, AvatarActor->GetActorRotation(), SpawnPrams);
		if (Projectile)
			Projectile->ShootProjectile(ProjectileSpeed, ProjectileRange, GetAimTargetIfValid(), GetOwnerTeamId(), MakeOutgoingGameplayEffectSpec(ProjectileHitEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)));
	}
}

AActor* UGA_Shoot::GetAimTargetIfValid() const
{
	if (HasValidTarget())
		return AimTarget;

	return nullptr;
}

void UGA_Shoot::FindAimTarget()
{
	if (HasValidTarget())
		return;

	if (AimTargetAbilitySystemComponent)
	{
		AimTargetAbilitySystemComponent->RegisterGameplayTagEvent(MGameplayTags::State_Dead).RemoveAll(this);
		AimTargetAbilitySystemComponent=nullptr;
	}

	AimTarget=GetAimTarget(ProjectileRange, ETeamAttitude::Hostile);
	if (AimTarget)
	{
		AimTargetAbilitySystemComponent=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AimTarget);
		if (AimTargetAbilitySystemComponent)
			AimTargetAbilitySystemComponent->RegisterGameplayTagEvent(MGameplayTags::State_Dead).AddUObject(this, &UGA_Shoot::TargetDeadTagUpdated);
	}

	FGameplayEventData EventData;
	EventData.Target=AimTarget;
	SendLocalGameplayEvent(MGameplayTags::Target_Updated, EventData);
	
}

void UGA_Shoot::StartAimTargetCheckTimer()
{
	UWorld* World=GetWorld();
	if (World)
		World->GetTimerManager().SetTimer(AimTargetCheckTimerHandle, this, &UGA_Shoot::FindAimTarget, AimTargetCheckTimeInterval, true);
}

void UGA_Shoot::StopAimTargetCheckTimer()
{
	UWorld* World = GetWorld();
	if (World)
		World->GetTimerManager().ClearTimer(AimTargetCheckTimerHandle);
}

bool UGA_Shoot::HasValidTarget() const
{
	if (!AimTarget) return false;

	if (UMAbilitySystemStatics::ActorHasTag(AimTarget, MGameplayTags::State_Dead)) return false;

	if (!IsTargetInRange()) return false;

	return true;
}

bool UGA_Shoot::IsTargetInRange() const
{
	if (!AimTarget) return false;

	float Distance = FVector::Distance(AimTarget->GetActorLocation(), GetAvatarActorFromActorInfo()->GetActorLocation());
	return Distance <= ProjectileRange;
}

void UGA_Shoot::TargetDeadTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount>0)
		FindAimTarget();
}
