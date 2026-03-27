// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MTypes/MEnumTypes.h"
#include "MAbilitySystemComponent.generated.h"

class UDataAsset_StartUpDataBase;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UMAbilitySystemComponent();

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	void InitializeBaseAttributes();
	bool IsAtMaxLevel() const;

	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);
	
	TObjectPtr<UDataAsset_StartUpDataBase> GetCharacterStartUpData() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpgradeAbilityWithTag(FGameplayTag AbilityTag);
	
	UFUNCTION(Client, Reliable)
	void Client_AbilitySpecLevelUpdated(FGameplayAbilitySpecHandle Handle, int32 NewLevel);

private:
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);
	void ExperienceUpdated(const FOnAttributeChangeData& ChangeData);
			
	UPROPERTY(EditDefaultsOnly, Category = "CharacterData")
	TObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;


	
};

