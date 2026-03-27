// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Widgets/MVVM/MVVM_AbilityEntry.h"
#include "PlayerStatsSubsystem.generated.h"

class UMAbilitySystemComponent;
class UMVVM_HeroStats;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UPlayerStatsSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void InitAbilityViewModels(UMAbilitySystemComponent* ASC, const TArray<FMPlayerAbilitySet>& AbilitySets);
	void InitPreviewAbilityViewModels(const TArray<FAbilityWidgetData*> WidgetDataList);

	void OnPawnPossessed(APawn* NewPawn);
	void OnPawnUnPossessed();

	UMVVM_HeroStats* GetPlayerStatsViewModel() const { return StatsViewModel; }
	TArray<TObjectPtr<UMVVM_AbilityEntry>> GetAbilityViewModels() const { return AbilityViewModels; }

	void SetAbilityDataTable(UDataTable* DataTable) { AbilityDataTable = DataTable; }

private:
	UPROPERTY()
	TObjectPtr<UMVVM_HeroStats> StatsViewModel;

	UPROPERTY()
	TArray<TObjectPtr<UMVVM_AbilityEntry>> AbilityViewModels;

	UPROPERTY()
	TObjectPtr<UDataTable> AbilityDataTable;

	UPROPERTY()
	TObjectPtr<UMAbilitySystemComponent> CachedASC;

	void BindToASC(UMAbilitySystemComponent* ASC);
	void UnbindFromASC();
	void ClearAbilityViewModels();

	const FAbilityWidgetData* FindWidgetData(const TSubclassOf<UGameplayAbility>& AbilityClass) const;


	
};
