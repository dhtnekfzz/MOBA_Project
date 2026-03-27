// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "MTypes/MEnumTypes.h"
#include "MGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MAbility")
	EMAbilityActivationPolicy AbilityActivationPolicy=EMAbilityActivationPolicy::OnTriggered;

protected:
	AActor* GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const;
	class UAnimInstance* GetOwnerAnimInstance() const;

	UFUNCTION()
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; }
	
	void PushSelf(const FVector& PushVel);
	void PushTarget(AActor* Target, const FVector& PushVel);
	void PushTargets(const TArray<AActor*>& Targets, const FVector& PushVel);
	void PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVel);
	void PushTargetsFromOwnerLocation(const TArray<AActor*>& Targets, float PushSpeed);
	void PushTargetsFromOwnerLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float PushSpeed);
	void PushTargetsFromLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& FromLocation, float PushSpeed);
	void PushTargetsFromLocation(const TArray<AActor*>& Targets, const FVector& FromLocation, float PushSpeed);
	void PlayMontageLocally(UAnimMontage* MontageToPlay);
	
	FGenericTeamId GetOwnerTeamId() const;
	
	bool IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const;

	ACharacter* GetOwningAvatarCharacter();
	void ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level=1);
	void SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
private:
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bShouldDrawDebug=false;

	UPROPERTY()
	class ACharacter* AvatarCharacter;

};
