// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor_GroundPick.h"
#include "MOBA_Project/MOBA_Project.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Components/DecalComponent.h"
#include "Engine/OverlapResult.h"


// Sets default values
ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root Comp"));

	DecalComp=CreateDefaultSubobject<UDecalComponent>("Decal Comp");

	DecalComp->SetupAttachment(GetRootComponent());
}

void ATargetActor_GroundPick::SetTargetAreaRadius(float NewRadius)
{
	TargetAreaRadius=NewRadius;
	DecalComp->DecalSize=FVector{NewRadius};
}

void ATargetActor_GroundPick::ConfirmTargetingAndContinue()
{
	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(TargetAreaRadius);

	GetWorld()->OverlapMultiByObjectType(OverlapResults, GetActorLocation(), FQuat::Identity, ObjectQueryParams, CollisionShape);

	TSet<AActor*> TargetActors;

	IGenericTeamAgentInterface* OwnerTeamInterface=nullptr;
	if (OwningAbility)
		OwnerTeamInterface=Cast<IGenericTeamAgentInterface>(OwningAbility->GetAvatarActorFromActorInfo());

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor())==ETeamAttitude::Friendly && !bShouldTargetFriendly)
			continue;

		if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor())==ETeamAttitude::Hostile && !bShouldTargetEnemy)
			continue;

		TargetActors.Add(OverlapResult.GetActor());
	}

	FGameplayAbilityTargetDataHandle TargetDataHandle=UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(TargetActors.Array(), false);

	FGameplayAbilityTargetData_SingleTargetHit* HitLoc=new FGameplayAbilityTargetData_SingleTargetHit;
	HitLoc->HitResult.ImpactPoint=GetActorLocation();

	TargetDataHandle.Add(HitLoc);

	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
	
}

void ATargetActor_GroundPick::SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy)
{
	bShouldTargetFriendly=bTargetFriendly;
	bShouldTargetFriendly=bTargetFriendly;
}

FVector ATargetActor_GroundPick::GetTargetPoint() const
{
	if (!PrimaryPC || !PrimaryPC->IsPlayerController()) return GetActorLocation();

	FHitResult TraceResult;

	FVector ViewLoc;
	FRotator ViewRot;

	PrimaryPC->GetPlayerViewPoint(ViewLoc, ViewRot);

	FVector TraceEnd=ViewLoc+ViewRot.Vector()*TargetTraceRange;

	GetWorld()->LineTraceSingleByChannel(TraceResult, ViewLoc, TraceEnd, ECC_Target);

	if (!TraceResult.bBlockingHit)
		GetWorld()->LineTraceSingleByChannel(TraceResult, ViewLoc, TraceEnd+FVector::DownVector*TNumericLimits<float>::Max(), ECC_Target);

	if (!TraceResult.bBlockingHit)
		return GetActorLocation();

	if (bShouldDrawDebug)
		DrawDebugSphere(GetWorld(), TraceResult.ImpactPoint, TargetAreaRadius, 32 , FColor::Red);

	return TraceResult.ImpactPoint;
	
}
void ATargetActor_GroundPick::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATargetActor_GroundPick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PrimaryPC && PrimaryPC->IsPlayerController())
		SetActorLocation(GetTargetPoint());
}

