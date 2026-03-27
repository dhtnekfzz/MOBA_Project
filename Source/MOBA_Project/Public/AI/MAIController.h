// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Perception/AIPerceptionTypes.h"
#include "MAIController.generated.h"

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API AMAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="AI Behavior")
	FName TargetBlackBoardKeyName="Target";

	UPROPERTY(EditDefaultsOnly, Category="AI Behavior")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category="Perception")
	class UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus);

	UFUNCTION()
	void TargetForgotten(AActor* ForgottenActor);

	const UObject* GetCurrentTarget() const;
	void SetCurrentTarget(AActor* NewTarget);

	AActor* GetNextPerceivedActor() const;

	void ForgetActorIfDead(AActor* ActorToForget);

	void ClearAndDisableAllSenses();
	void EnableAllSenses();

	void PawnDeadTagUpdated(const FGameplayTag Tag, int32 Count);
	void PawnStunTagUpdated(const FGameplayTag Tag, int32 Count);

	bool bIsPawnDead=false;
	
	
};
