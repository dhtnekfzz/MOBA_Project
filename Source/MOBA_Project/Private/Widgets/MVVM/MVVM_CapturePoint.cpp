// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MVVM/MVVM_CapturePoint.h"

#include "Components/SlateWrapperTypes.h"

void UMVVM_CapturePoint::UpdateFromCapturePoint(float Progress, ECapturePointState State,float VictoryTime,FLinearColor InTeamOneColor,FLinearColor InTeamTwoColor)
{
	SetTeamOneProgress(FMath::Max(0.f, Progress)*100);
	SetTeamTwoProgress(FMath::Max(0.f, -Progress)*100);

	SetCaptureState(State);
	SetVictoryTimeRemaining(VictoryTime);
	SetTeamOneColor(InTeamOneColor);
	SetTeamTwoColor(InTeamTwoColor);
}

ESlateVisibility UMVVM_CapturePoint::GetVictoryTimerVisibility() const
{
	if (CaptureState == ECapturePointState::TeamOneCaptured ||CaptureState == ECapturePointState::TeamTwoCaptured)
	{
		return ESlateVisibility::Visible;
	}
	
	if (CaptureState == ECapturePointState::Contested && VictoryTimeRemaining > 0.f)
		return ESlateVisibility::Visible;

	return ESlateVisibility::Hidden;
}


void UMVVM_CapturePoint::SetTeamOneProgress(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(TeamOneProgress, NewValue);
}


void UMVVM_CapturePoint::SetTeamTwoProgress(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(TeamTwoProgress, NewValue);
}

void UMVVM_CapturePoint::SetCaptureState(ECapturePointState NewValue)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(CaptureState, NewValue))
	{
		UpdateStateText();
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetVictoryTimerVisibility);
	}
}

void UMVVM_CapturePoint::SetVictoryTimeRemaining(float NewValue)
{
	if (NewValue<0) return;
	UE_MVVM_SET_PROPERTY_VALUE(VictoryTimeRemaining, NewValue);
}

void UMVVM_CapturePoint::SetTeamOneColor(const FLinearColor& NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(TeamOneColor, NewValue);
}

void UMVVM_CapturePoint::SetTeamTwoColor(const FLinearColor& NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(TeamTwoColor, NewValue);
}

void UMVVM_CapturePoint::SetStateText(const FText& NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(StateText, NewValue);
}

void UMVVM_CapturePoint::UpdateStateText()
{
	FText NewText;
	switch (CaptureState)
	{
	case ECapturePointState::Neutral:
		NewText = FText::FromString("Neutral");   break;
	case ECapturePointState::TeamOneCapturing:
	case ECapturePointState::TeamTwoCapturing:
		NewText = FText::FromString("Capturing"); break;
	case ECapturePointState::TeamOneCaptured:
	case ECapturePointState::TeamTwoCaptured:
		NewText = FText::FromString("Captured");  break;
	case ECapturePointState::Contested:
		NewText = FText::FromString("Contested"); break;
	default:
		NewText = FText::FromString("Neutral");   break;
	}
	SetStateText(NewText);
}
