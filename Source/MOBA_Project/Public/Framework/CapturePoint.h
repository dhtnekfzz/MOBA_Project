// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "CapturePoint.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCapturePointVictory, int32 /*WinningTeamId*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCaptureStateChanged, int32 /*TeamOneCount*/, int32 /*TeamTwoCount*/);

UENUM(BlueprintType)
enum class ECapturePointState : uint8
{
	Neutral,
	TeamOneCapturing,
	TeamOneCaptured,
	TeamTwoCapturing,
	TeamTwoCaptured,
	Contested,
};

UCLASS()
class MOBA_PROJECT_API ACapturePoint : public AActor
{
	GENERATED_BODY()

public:
	ACapturePoint();

	FOnCapturePointVictory OnCapturePointVictory;
	FOnCaptureStateChanged OnCaptureStateChanged;

	UFUNCTION(BlueprintPure, Category="CapturePoint")
	float GetCaptureProgress() const { return CaptureProgress; }

	UFUNCTION(BlueprintPure, Category="CapturePoint")
	ECapturePointState GetCaptureState() const { return CaptureState; }

	UFUNCTION(BlueprintPure, Category="CapturePoint")
	FLinearColor GetCurrentTeamColor() const;

	UFUNCTION(BlueprintPure, Category="CapturePoint")
	FORCEINLINE FLinearColor GetTeamOneColor() const { return TeamOneColor; }

	UFUNCTION(BlueprintPure, Category="CapturePoint")
	FORCEINLINE FLinearColor GetTeamTwoColor() const { return TeamTwoColor; } 
	
	UFUNCTION(BlueprintPure, Category="CapturePoint")
	float GetVictoryTimeRemaining() const { return VictoryTimeRemaining; }
	
	UFUNCTION(BlueprintImplementableEvent, Category="CapturePoint")
	void OnCaptureCompleted(FLinearColor WinningTeamColor, int32 WinningTeamId);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Detection")
	class UBoxComponent* CaptureZone;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Visual")
	class UNiagaraComponent* NiagaraEffect;

	UPROPERTY(VisibleDefaultsOnly, Category="Visual")
	class UCameraComponent* ViewCamera;

	UPROPERTY(EditDefaultsOnly, Category="CapturePoint")
	FVector CaptureExtent = FVector(600.f, 600.f, 200.f);

	UPROPERTY(EditDefaultsOnly, Category="CapturePoint", meta=(ClampMin="0.01"))
	float CaptureRate = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category="CapturePoint", meta=(ClampMin="1.0"))
	float VictoryHoldDuration = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Niagara")
	FName NiagaraParamProgress = "CaptureProgress";

	UPROPERTY(EditDefaultsOnly, Category="Niagara")
	FName NiagaraParamTeamColor = "TeamColor";
	
	UPROPERTY(EditDefaultsOnly, Category="TeamColor")
	FLinearColor TeamOneColor = FLinearColor(0.f, 0.4f, 1.f, 1.f);   // 파란색

	UPROPERTY(EditDefaultsOnly, Category="TeamColor")
	FLinearColor TeamTwoColor = FLinearColor(1.f, 0.2f, 0.f, 1.f);   // 빨간색

	UPROPERTY(EditDefaultsOnly, Category="TeamColor")
	FLinearColor NeutralColor = FLinearColor(1.f, 1.f, 1.f, 1.f);    // 흰색

	UPROPERTY(EditDefaultsOnly, Category="TeamColor")
	FLinearColor ContestedColor = FLinearColor(1.f, 0.8f, 0.f, 1.f); // 노란색
	
	UPROPERTY(ReplicatedUsing=OnRep_CaptureState)
	ECapturePointState CaptureState = ECapturePointState::Neutral;

	UPROPERTY(ReplicatedUsing=OnRep_CaptureProgress)
	float CaptureProgress = 0.f;

	UPROPERTY(Replicated)
	float VictoryTimeRemaining = 0.f;

	UFUNCTION()
	void OnRep_CaptureState();

	UFUNCTION()
	void OnRep_CaptureProgress();

	ECapturePointState PrevCaptureState = ECapturePointState::Neutral;

	int32 TeamOneCount = 0;
	int32 TeamTwoCount = 0;
	bool  bVictoryDeclared = false;

	UFUNCTION()
	void OnActorEnterZone(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnActorLeaveZone(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateCaptureState();
	void TickCapture(float DeltaTime);
	void TickVictoryTimer(float DeltaTime);
	void OnVictoryTimerExpired(int32 WinningTeamId);

	void UpdateNiagaraParameters();
	
	float GetNormalizedProgress() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
