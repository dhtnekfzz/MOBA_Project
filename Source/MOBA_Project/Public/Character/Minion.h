// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Character/MCharacter.h"
#include "Minion.generated.h"

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API AMinion : public AMCharacter
{
	GENERATED_BODY()
	
public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	bool IsActive() const;
	void Activate();
	void SetGoal(AActor* Goal);

private:
	void PickSkinBasedOnTeamId();

	virtual void OnRep_TeamID() override;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	FName GoalBlackboardKeyName="Goal";

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	TMap<FGenericTeamId, USkeletalMesh*> SkinMap;
	
};
