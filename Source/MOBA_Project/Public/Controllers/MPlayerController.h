// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "MPlayerController.generated.h"

class UCommonActivatableWidget;
class UPrimaryLayout;
class AMPlayerCharacter;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API AMPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// 서버에서만 호출
	virtual void OnPossess(APawn* InPawn) override;

	// 클라이언트, 리슨 서버에서만 실행
	virtual void AcknowledgePossession(class APawn* NewPawn) override;

	// IGenericTeamAgentInterface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	
#pragma region ModelViewModel
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<class UWidgetComponent> OverHeadWidgetComponent;

	UPROPERTY()
	TObjectPtr<class UMVVM_HeroStats> HeroStatsViewModel;

#pragma endregion 

private:
	UPROPERTY()
	TObjectPtr<AMPlayerCharacter> MPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UPrimaryLayout> PrimaryLayoutClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UGameplayWidget> GameplayWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UShopWidget> ShopWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UMGameplayMenuWidget> GameplayMenuWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	TSubclassOf<class UMouseModeWidget> MouseModeWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	TSubclassOf<class UMRespawnWidget> RespawnWidgetClass;
	
	UFUNCTION(Client, Reliable)
	void Client_MatchFinished(AActor* ViewTarget, int32 WinningTeamId);

	UPROPERTY(EditDefaultsOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	float MatchFinishViewBlendTime = 2.f;

public:
	void ToggleShopWidget();
	void ToggleGameplayMenuWidget();
	void PushWidget(TSubclassOf<UCommonActivatableWidget> WidgetClass);


	void MatchFinished(AActor* ViewTarget, int32 WinningTeamId);
	void EnableMouseMode();
	
	void DisableMouseMode();
	
	UPROPERTY()
	class UGameplayWidget* GameplayWidget;

	UPROPERTY(EditDefaultsOnly, Category="Data")
	TObjectPtr<UDataTable> AbilityDataTable;

	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
};
