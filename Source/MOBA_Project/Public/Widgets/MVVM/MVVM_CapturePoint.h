// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Framework/CapturePoint.h"
#include "MVVM_CapturePoint.generated.h"

UCLASS()
class MOBA_PROJECT_API UMVVM_CapturePoint : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void UpdateFromCapturePoint(float Progress, ECapturePointState State,float VictoryTime,FLinearColor InTeamOneColor,FLinearColor InTeamTwoColor);

	float GetTeamOneProgress() const { return TeamOneProgress; }
	float GetTeamTwoProgress() const { return TeamTwoProgress; }
	ECapturePointState GetCaptureState() const { return CaptureState; }
	float GetVictoryTimeRemaining() const { return VictoryTimeRemaining; }
	FLinearColor GetTeamOneColor() const { return TeamOneColor; }
	FLinearColor GetTeamTwoColor() const { return TeamTwoColor; }
	FText GetStateText() const { return StateText; }
	
	UFUNCTION(BlueprintPure, FieldNotify)
	ESlateVisibility GetVictoryTimerVisibility() const;

	void SetTeamOneProgress(float NewValue);
	void SetTeamTwoProgress(float NewValue);
	void SetCaptureState(ECapturePointState NewValue);
	void SetVictoryTimeRemaining(float NewValue);
	void SetTeamOneColor(const FLinearColor& NewValue);
	void SetTeamTwoColor(const FLinearColor& NewValue);

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	float TeamOneProgress = 0.f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	float TeamTwoProgress = 0.f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	ECapturePointState CaptureState = ECapturePointState::Neutral;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	float VictoryTimeRemaining = 0.f;
	
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FLinearColor TeamOneColor = FLinearColor(0.f, 0.4f, 1.f, 1.f); 

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FLinearColor TeamTwoColor = FLinearColor(1.f, 0.2f, 0.f, 1.f); 

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	FText StateText = FText::FromString("Neutral");

	void SetStateText(const FText& NewValue);
	void UpdateStateText();
};
