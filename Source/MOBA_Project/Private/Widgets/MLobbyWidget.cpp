// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MLobbyWidget.h"
#include "Framework/MAssetManager.h"
#include "Framework/MGameState.h"
#include "Player/MPlayerState.h"
#include "Controllers/MLobbyPlayerController.h"
#include "DataAssets/DA_MCharacterDefinition.h"
#include "Network/MNetStatics.h"
#include "Widgets/MTeamSelectionWidget.h"
#include "Widgets/MCharacterEntryWidget.h"
#include "Widgets/MCharacterDisplay.h"
#include "Widgets/MPlayerTeamLayoutWidget.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "Components/WidgetSwitcher.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/AbilityListView.h"
#include "Widgets/Subsystems/PlayerStatsSubsystem.h"


void UMLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LobbyController = GetOwningPlayer<AMLobbyPlayerController>();
	MPlayerState = GetOwningPlayerState<AMPlayerState>();

	if (LobbyController)
	{
		LobbyController->OnSwitchToHeroSelection.BindUObject(this, &UMLobbyWidget::SwitchToHeroSelection);
	}
	
	PopulateTeamSelectionSlots();
	TryBindGameState();


	UMAssetManager::Get().LoadCharacterDefinitions(FStreamableDelegate::CreateUObject(this, &UMLobbyWidget::OnCharacterDefinitionsLoaded));

	if (CharacterSelectionTileView)
		CharacterSelectionTileView->OnItemSelectionChanged().AddUObject(this, &UMLobbyWidget::OnCharacterSelected);

	SpawnCharacterDisplay();

	if (StartHeroSelectionButton)
	{
		StartHeroSelectionButton->SetIsEnabled(false);
		StartHeroSelectionButton->OnClicked.AddDynamic(this, &UMLobbyWidget::OnStartHeroSelectionClicked);
	}

	if (StartMatchButton)
	{
		StartMatchButton->SetIsEnabled(false);
		StartMatchButton->OnClicked.AddDynamic(this, &UMLobbyWidget::OnStartMatchClicked);
	}
}


void UMLobbyWidget::TryBindGameState()
{
	MGameState = GetWorld() ? GetWorld()->GetGameState<AMGameState>() : nullptr;

	if (!MGameState)
	{
		GetWorld()->GetTimerManager().SetTimer(GameStateRetryHandle, this, &UMLobbyWidget::TryBindGameState, 1.f);
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(GameStateRetryHandle);


	MGameState->OnPlayerSelectionUpdated.AddUObject(this, &UMLobbyWidget::OnPlayerSelectionUpdated);
	UpdatePlayerSelectionDisplay(MGameState->GetPlayerSelection());
}


void UMLobbyWidget::PopulateTeamSelectionSlots()
{
	if (!TeamSelectionSlotGridPanel || !TeamSelectionWidgetClass) return;

	TeamSelectionSlotGridPanel->ClearChildren();
	TeamSelectionSlots.Empty();

	const int32 CountPerTeam = UMNetStatics::GetPlayerCountPerTeam();
	const int32 Total        = CountPerTeam * 2;

	for (int32 i = 0; i < Total; ++i)
	{
		UMTeamSelectionWidget* SlotWidget =CreateWidget<UMTeamSelectionWidget>(GetOwningPlayer(), TeamSelectionWidgetClass);
		if (!SlotWidget) continue;

		SlotWidget->SetSlotID(static_cast<uint8>(i));
		SlotWidget->OnMSlotClicked.AddUObject(this, &UMLobbyWidget::OnSlotClicked);
		TeamSelectionSlots.Add(SlotWidget);

		if (UUniformGridSlot* GridSlot =
			TeamSelectionSlotGridPanel->AddChildToUniformGrid(SlotWidget))
		{
			GridSlot->SetColumn(i < CountPerTeam ? 0 : 1);
			GridSlot->SetRow(i % CountPerTeam);
		}
	}
}


void UMLobbyWidget::OnSlotClicked(uint8 NewSlotId)
{
	if (LobbyController)
		LobbyController->Server_RequestSlotChange(NewSlotId);
}


void UMLobbyWidget::OnCharacterDefinitionsLoaded()
{
	TArray<UDA_MCharacterDefinition*> Definitions;
	if (!UMAssetManager::Get().GetLoadedCharacterDefinitions(Definitions)) return;

	if (CharacterSelectionTileView)
	{
		TArray<UObject*> Objects;
		Objects.Reserve(Definitions.Num());
		for (UDA_MCharacterDefinition* Def : Definitions)
			Objects.Add(Def);

		CharacterSelectionTileView->SetListItems(Objects);
	}
}

void UMLobbyWidget::OnCharacterSelected(UObject* SelectedObject)
{
	if (!MPlayerState)
		MPlayerState = GetOwningPlayerState<AMPlayerState>();
	if (!MPlayerState) return;

	if (const UDA_MCharacterDefinition* Def =
		Cast<UDA_MCharacterDefinition>(SelectedObject))
	{
		MPlayerState->Server_SetSelectedCharacterDefinition(Def);
	}
}

void UMLobbyWidget::SpawnCharacterDisplay()
{
	if (CharacterDisplay || !CharacterDisplayClass) return;

	FTransform SpawnTransform = FTransform::Identity;
	if (AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()))
		SpawnTransform = PlayerStart->GetActorTransform();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride =ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CharacterDisplay = GetWorld()->SpawnActor<AMCharacterDisplay>(CharacterDisplayClass, SpawnTransform, Params);

	if (CharacterDisplay && GetOwningPlayer())
		GetOwningPlayer()->SetViewTarget(CharacterDisplay);
}


void UMLobbyWidget::UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection)
{
	if (!CharacterDisplay) return;
	const UDA_MCharacterDefinition* Def = PlayerSelection.GetCharacterDefinition();
	if (!Def) return;

	CharacterDisplay->ConfigureWithDefinition(Def);
	AbilityListView->ClearListItems();

	UPlayerStatsSubsystem* StatsSub=GetOwningLocalPlayer()->GetSubsystem<UPlayerStatsSubsystem>();
	if (!StatsSub) return;

	StatsSub->InitPreviewAbilityViewModels(Def->GetAbilityWidgetDataList());
	
	AbilityListView->ConfigureWithViewModels(StatsSub->GetAbilityViewModels());

	
}

void UMLobbyWidget::OnPlayerSelectionUpdated(const TArray<FPlayerSelection>& PlayerSelections)
{
	UpdatePlayerSelectionDisplay(PlayerSelections);
}

void UMLobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections)
{
	for (UMTeamSelectionWidget* EmptySlot : TeamSelectionSlots)
		EmptySlot->UpdateSlotInfo("Empty");


	for (UUserWidget* EntryWidget : CharacterSelectionTileView->GetDisplayedEntryWidgets())
	{
		if (UMCharacterEntryWidget* Entry =Cast<UMCharacterEntryWidget>(EntryWidget))
		{
			Entry->SetSelected(false);
		}
	}
	
	for (const FPlayerSelection& Sel : PlayerSelections)
	{
		if (!Sel.IsValid()) continue;

		const uint8 SlotIdx = Sel.GetPlayerSlot();
		if (TeamSelectionSlots.IsValidIndex(SlotIdx))
			TeamSelectionSlots[SlotIdx]->UpdateSlotInfo(Sel.GetPlayerNickName());
		
		if (UMCharacterEntryWidget* Entry = CharacterSelectionTileView->GetEntryWidgetFromItem<UMCharacterEntryWidget>(const_cast<UDA_MCharacterDefinition*>(Sel.GetCharacterDefinition())))
		{
			Entry->SetSelected(true);
		}
		
		if (MPlayerState && Sel.IsForPlayer(MPlayerState))
			UpdateCharacterDisplay(Sel);
	}

	if (MGameState)
	{
		if (StartHeroSelectionButton)
			StartHeroSelectionButton->SetIsEnabled(MGameState->CanStartHeroSelection());
		if (StartMatchButton)
			StartMatchButton->SetIsEnabled(MGameState->CanStartMatch());
	}

	if (PlayerTeamLayoutWidget)
		PlayerTeamLayoutWidget->UpdatePlayerSelection(PlayerSelections);
}


void UMLobbyWidget::OnStartHeroSelectionClicked()
{
	if (LobbyController)
		LobbyController->Server_StartHeroSelection();
}


void UMLobbyWidget::SwitchToHeroSelection()
{
	if (MainSwitcher && HeroSelectionRoot)
		MainSwitcher->SetActiveWidget(HeroSelectionRoot);
}


void UMLobbyWidget::OnStartMatchClicked()
{
	if (LobbyController)
		LobbyController->Server_RequestStartMatch();
}
