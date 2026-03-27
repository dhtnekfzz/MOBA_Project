// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/MActivatableWidget.h"
#include "MTypes/MStructTypes.h"
#include "MLobbyWidget.generated.h"

class AMGameState;
class AMPlayerState;
class AMLobbyPlayerController;
class AMCharacterDisplay;
class UDA_MCharacterDefinition;


UCLASS()
class MOBA_PROJECT_API UMLobbyWidget : public UMMenuActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* MainSwitcher;

	UPROPERTY(meta=(BindWidget))
	class UWidget* TeamSelectionRoot;

	UPROPERTY(meta=(BindWidget))
	class UWidget* HeroSelectionRoot;

	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* TeamSelectionSlotGridPanel;

	UPROPERTY(meta=(BindWidget))
	class UButton* StartHeroSelectionButton;

	UPROPERTY(EditDefaultsOnly, Category="TeamSelection")
	TSubclassOf<class UMTeamSelectionWidget> TeamSelectionWidgetClass;

	UPROPERTY()
	TArray<TObjectPtr<class UMTeamSelectionWidget>> TeamSelectionSlots;

	void PopulateTeamSelectionSlots();
	void OnSlotClicked(uint8 NewSlotId);

	UPROPERTY(meta=(BindWidget))
	class UTileView* CharacterSelectionTileView;

	UPROPERTY(meta=(BindWidget))
	class UMPlayerTeamLayoutWidget* PlayerTeamLayoutWidget;

	UPROPERTY(meta=(BindWidget))
	class UButton* StartMatchButton;

	UPROPERTY(meta=(BindWidget))
	class UAbilityListView* AbilityListView;

	void OnCharacterDefinitionsLoaded();

	void OnCharacterSelected(UObject* SelectedObject);

	UPROPERTY(EditDefaultsOnly, Category="Display")
	TSubclassOf<class AMCharacterDisplay> CharacterDisplayClass;

	UPROPERTY()
	TObjectPtr<class AMCharacterDisplay> CharacterDisplay;

	void SpawnCharacterDisplay();
	void UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection);

	UPROPERTY()
	TObjectPtr<AMGameState> MGameState;

	UPROPERTY()
	TObjectPtr<AMPlayerState> MPlayerState;

	void TryBindGameState();
	FTimerHandle GameStateRetryHandle;

	void OnPlayerSelectionUpdated(const TArray<FPlayerSelection>& PlayerSelections);
	void UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections);

	UPROPERTY()
	TObjectPtr<AMLobbyPlayerController> LobbyController;

	UFUNCTION()
	void OnStartHeroSelectionClicked();

	UFUNCTION()
	void OnStartMatchClicked();
	
	void SwitchToHeroSelection();
};
