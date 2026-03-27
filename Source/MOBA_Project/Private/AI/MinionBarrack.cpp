// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MinionBarrack.h"

#include "Character/Minion.h"
#include "GameFramework/PlayerStart.h"


// Sets default values
AMinionBarrack::AMinionBarrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &ThisClass::SpawnNewGroup, GroupSpawnInterval, true);
	
}

// Called every frame
void AMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const APlayerStart* AMinionBarrack::GetNextSpawnSpot()
{
	if (SpawnSpots.IsEmpty()) return nullptr;

	++NextSpawnSpotIndex;

	if (NextSpawnSpotIndex >= SpawnSpots.Num())
		NextSpawnSpotIndex = 0;

	return SpawnSpots[NextSpawnSpotIndex];
}

void AMinionBarrack::SpawnNewGroup()
{
	int i=MinionPerGroup;

	while (i>0)
	{
		FTransform SpawnTransform=GetActorTransform();
		
		if (const APlayerStart* NextSpawnSpot=GetNextSpawnSpot())
			SpawnTransform=NextSpawnSpot->GetActorTransform();

		AMinion* NextAvailableMinion=GetNextAvailableMinion();
		if (!NextAvailableMinion) break;

		NextAvailableMinion->SetActorTransform(SpawnTransform);
		NextAvailableMinion->Activate();
		--i;
	}

	SpawnNewMinion(i);
}

void AMinionBarrack::SpawnNewMinion(int Amt)
{
	for (int i = 0; i < Amt; i++)
	{
		FTransform SpawnTransform = GetActorTransform();
		
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		
		AMinion* NewMinion = GetWorld()->SpawnActorDeferred<AMinion>(MinionClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		NewMinion->SetGenericTeamId(BarrackTeamId);
		NewMinion->FinishSpawning(SpawnTransform);
		NewMinion->SetGoal(Goal);
		MinionPool.Add(NewMinion);
	}
}

AMinion* AMinionBarrack::GetNextAvailableMinion() const
{
	for (AMinion* Minion : MinionPool)
	{
		if (!Minion->IsActive())
			return Minion;
	}

	return nullptr;
}

void AMinionBarrack::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnIntervalTimerHandle);
}

