// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GenericTeamAgentInterface.h"
#include "MTypes/MStructTypes.h"
#include "MPlayerState.generated.h"

class UDA_MCharacterDefinition;
class AMGameState;

UCLASS()
class MOBA_PROJECT_API AMPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void BeginPlay() override;
	
	TSubclassOf<APawn> GetSelectedPawnClass() const;
	FGenericTeamId GetTeamIdBasedOnSlot() const;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSelectedCharacterDefinition(const UDA_MCharacterDefinition* NewDefinition);

	void SetPlayerNickname(bool bSuccess,const FString& NickName, const FString& Msg);

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerNickName(const FString& NickName);

private:
	UPROPERTY(Replicated)
	FPlayerSelection PlayerSelection;

	UPROPERTY()
	TObjectPtr<AMGameState> MGameState;

	void OnPlayerSelectionUpdated(const TArray<FPlayerSelection>& NewSelections);
};

