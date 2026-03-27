// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "MTypes/MStructTypes.h"
#include "MCharacter.generated.h"

class UMVVM_Gauge;
class UDataAsset_StartUpDataBase;
class UMAbilitySystemComponent;
class UMAttributeSet;

UCLASS()
class MOBA_PROJECT_API AMCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMCharacter();
	void ServerSideInit();
	void ClientSideInit();
	bool IsLocallyControlledByPlayer() const;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

#pragma region UI
private:
	void InitializeUI();

	void ConfigureOverHeadStatWidget();
	
	UPROPERTY(VisibleDefaultsOnly, Category="UI")
	TObjectPtr<class UWidgetComponent> OverHeadWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	float HeadStatGaugeVisibilityCheckUpdateGap=1.f;
 
	UPROPERTY(EditDefaultsOnly, Category="UI")
	float HeadStatGaugeVisibilityRangeSquared=10000000.f;

	UPROPERTY()
	TObjectPtr<UMVVM_Gauge> GuageMV;

	FTimerHandle HeadStatGaugeVisibilityUpdateTimerHandle;

	void UpdateHeadStatGaugeVisibility();
	void SetStatGaugeEnabled(bool bIsEnabled);


#pragma endregion 

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Gameplay Ability
	
public:
	// AbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendGameplayEventToSelf(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
	
	TArray<FMPlayerAbilitySet> GetAbilitySet();

protected:
	void UpgradeAbilityWithInputTag(FGameplayTag& InInputTag);

	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	UMAbilitySystemComponent* ASC;

private:
	UPROPERTY()
	UMAttributeSet* MAttributeSet;

	
private:
	void BindGASChangeDelegates();
	void DeathTagUpdated(const FGameplayTag Tag, int32 NewCount);
	void StunTageUpdated(const FGameplayTag Tag, int32 NewCount);
	void AimTagUpdated(const FGameplayTag Tag, int32 NewCount);

	void MoveSpeedUpdated(const FOnAttributeChangeData& Data);
	void MoveSpeedAccelerationUpdated(const FOnAttributeChangeData& Data);

	void SetIsAiming(bool bIsAiming);
	virtual void OnAimStateChanged(bool bIsAiming);


#pragma endregion

#pragma region Stun

private:
	UPROPERTY(EditDefaultsOnly, Category="Stun")
	TObjectPtr<UAnimMontage> StunMontage;

	virtual void OnStun();
	virtual void OnRecoverFromStun();


#pragma endregion

	
#pragma region Death and Respawn

public:
	bool IsDead() const;
	void RespawnImmediately();

private:
	FTransform MeshRelativeTransform;

	UPROPERTY(EditDefaultsOnly, Category="Death")
	float DeathMontageEndTimeShift=-0.8f;

	UPROPERTY(EditDefaultsOnly, Category="Death")
	TObjectPtr<UAnimMontage> DeathMontage;

	FTimerHandle DeathMontageTimerHandle;

	void DeathMontageEnded();
	void SetRagdollEnabled(bool bIsEnabled);

	void PlayDeathAnimation();

	void StartDeathSequence();
	void Respawn();

	virtual void OnDead();
	virtual void OnRespawn();
	
#pragma endregion

#pragma region Team


public:
	// IGenericTeamAgentInterface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	virtual FGenericTeamId GetGenericTeamId() const override;

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamID)
	FGenericTeamId TeamID;

	UFUNCTION()
	virtual void OnRep_TeamID();
	
#pragma endregion

#pragma region AI

private:
	void SetAIPerceptionStimuliSourceEnable(bool bIsEnabled);

	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComponent;
	
#pragma endregion
	
};



