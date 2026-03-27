// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_GroundPick.generated.h"

UCLASS()
class MOBA_PROJECT_API ATargetActor_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ATargetActor_GroundPick();

	void SetTargetAreaRadius(float NewRadius);
	
	FORCEINLINE void SetTargetTraceRange(float NewRange) { TargetTraceRange=NewRange; }
	FORCEINLINE void SetShouldDrawDebug(bool bDrawDebug) { bShouldDrawDebug=bDrawDebug; }

	virtual void ConfirmTargetingAndContinue() override;
	void SetTargetOptions(bool bTargetFriendly, bool bTargetEnemy=true);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY(VisibleDefaultsOnly, Category="Visual")
	class UDecalComponent* DecalComp;

	bool bShouldTargetEnemy= true;
	bool bShouldTargetFriendly= false;

	virtual void Tick(float DeltaTime) override;

	FVector GetTargetPoint() const;

	UPROPERTY(VisibleDefaultsOnly, Category="Targeting")
	float TargetAreaRadius=300.f;

	UPROPERTY(VisibleDefaultsOnly, Category="Targeting")
	float TargetTraceRange= 20000.f;

	bool bShouldDrawDebug=false;
	
};
