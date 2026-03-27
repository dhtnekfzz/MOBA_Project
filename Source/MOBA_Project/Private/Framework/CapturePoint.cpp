// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/CapturePoint.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ACapturePoint::ACapturePoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CaptureZone = CreateDefaultSubobject<UBoxComponent>("CaptureZone");
	SetRootComponent(CaptureZone);
	CaptureZone->SetBoxExtent(CaptureExtent);
	CaptureZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	CaptureZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CaptureZone->OnComponentBeginOverlap.AddDynamic(this, &ACapturePoint::OnActorEnterZone);
	CaptureZone->OnComponentEndOverlap.AddDynamic(this, &ACapturePoint::OnActorLeaveZone);

	NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>("NiagaraEffect");
	NiagaraEffect->SetupAttachment(RootComponent);

	ViewCamera = CreateDefaultSubobject<UCameraComponent>("ViewCamera");
	ViewCamera->SetupAttachment(RootComponent);
}

void ACapturePoint::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ACapturePoint, CaptureState, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ACapturePoint, CaptureProgress, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(ACapturePoint, VictoryTimeRemaining);
}

void ACapturePoint::BeginPlay()
{
	Super::BeginPlay();

	UpdateNiagaraParameters();
}

void ACapturePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority()) return;

	TickCapture(DeltaTime);
}

void ACapturePoint::OnActorEnterZone(UPrimitiveComponent* Overlapped,AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor) return;

	IGenericTeamAgentInterface* TeamInterface = Cast<IGenericTeamAgentInterface>(OtherActor);
	if (!TeamInterface) return;

	const uint8 TeamId = TeamInterface->GetGenericTeamId().GetId();
	if(TeamId == 0) TeamOneCount++;
	else if(TeamId == 1) TeamTwoCount++;

	UpdateCaptureState();
	OnCaptureStateChanged.Broadcast(TeamOneCount, TeamTwoCount);
}


void ACapturePoint::OnActorLeaveZone(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority() || !OtherActor) return;

	IGenericTeamAgentInterface* TeamInterface =Cast<IGenericTeamAgentInterface>(OtherActor);
	if (!TeamInterface) return;

	const uint8 TeamId = TeamInterface->GetGenericTeamId().GetId();
	if (TeamId == 0) TeamOneCount = FMath::Max(0, TeamOneCount - 1);
	else if (TeamId == 1) TeamTwoCount = FMath::Max(0, TeamTwoCount - 1);

	UpdateCaptureState();
	OnCaptureStateChanged.Broadcast(TeamOneCount, TeamTwoCount);
}

void ACapturePoint::UpdateCaptureState()
{
	if (bVictoryDeclared) return;

	const bool bT1 = TeamOneCount > 0;
	const bool bT2 = TeamTwoCount > 0;

	if (bT1 && bT2)
	{
		CaptureState = ECapturePointState::Contested;
	}
	else if (bT1)
	{
		CaptureState = (CaptureProgress >= 1.f) ? ECapturePointState::TeamOneCaptured : ECapturePointState::TeamOneCapturing;
	}
	else if (bT2)
	{
		CaptureState = (CaptureProgress <= -1.f) ? ECapturePointState::TeamTwoCaptured : ECapturePointState::TeamTwoCapturing;
	}
	else
	{
		if(CaptureProgress >= 1.f)  CaptureState = ECapturePointState::TeamOneCaptured;
		else if(CaptureProgress <= -1.f) CaptureState = ECapturePointState::TeamTwoCaptured;
		else CaptureState = ECapturePointState::Neutral;
	}
}


void ACapturePoint::TickCapture(float DeltaTime)
{
	const float PrevProgress = CaptureProgress;

	switch (CaptureState)
	{
	case ECapturePointState::TeamOneCapturing:
		CaptureProgress = FMath::Min(1.f, CaptureProgress + CaptureRate * DeltaTime);
		if (CaptureProgress >= 1.f)
		{
			CaptureState = ECapturePointState::TeamOneCaptured;
			VictoryTimeRemaining = VictoryHoldDuration;

			OnCaptureCompleted(TeamOneColor, 0);
		}
		break;

	case ECapturePointState::TeamTwoCapturing:
		CaptureProgress = FMath::Max(-1.f, CaptureProgress - CaptureRate * DeltaTime);
		if (CaptureProgress <= -1.f)
		{
			CaptureState = ECapturePointState::TeamTwoCaptured;
			VictoryTimeRemaining = VictoryHoldDuration;
			OnCaptureCompleted(TeamTwoColor, 1);
		}
		break;

	case ECapturePointState::TeamOneCaptured:
	case ECapturePointState::TeamTwoCaptured:
		TickVictoryTimer(DeltaTime);
		break;

	case ECapturePointState::Neutral:
		if(CaptureProgress > 0.f)
			CaptureProgress = FMath::Max(0.f, CaptureProgress - CaptureRate * DeltaTime);
		else if (CaptureProgress < 0.f)
			CaptureProgress = FMath::Min(0.f, CaptureProgress + CaptureRate * DeltaTime);
		break;

	case ECapturePointState::Contested:
		break;

	default: break;
	}
	
	if (!FMath::IsNearlyEqual(CaptureProgress, PrevProgress))
		UpdateNiagaraParameters();
}

void ACapturePoint::TickVictoryTimer(float DeltaTime)
{
	VictoryTimeRemaining -= DeltaTime;

	if (VictoryTimeRemaining <= 0.f)
	{
		const int32 WinningTeamId =(CaptureState == ECapturePointState::TeamOneCaptured) ? 0 : 1;
		OnVictoryTimerExpired(WinningTeamId);
	}
}


void ACapturePoint::OnVictoryTimerExpired(int32 WinningTeamId)
{
	if (bVictoryDeclared) return;
	bVictoryDeclared = true;

	SetActorTickEnabled(false);
	OnCapturePointVictory.Broadcast(WinningTeamId);
}

void ACapturePoint::OnRep_CaptureState()
{
	UpdateNiagaraParameters();

	const bool bJustCapturedByT1 = PrevCaptureState == ECapturePointState::TeamOneCapturing && CaptureState == ECapturePointState::TeamOneCaptured;

	const bool bJustCapturedByT2 = PrevCaptureState == ECapturePointState::TeamTwoCapturing && CaptureState == ECapturePointState::TeamTwoCaptured;

	if (bJustCapturedByT1)
		OnCaptureCompleted(TeamOneColor, 0);
	else if (bJustCapturedByT2)
		OnCaptureCompleted(TeamTwoColor, 1);

	PrevCaptureState = CaptureState;
}

void ACapturePoint::OnRep_CaptureProgress()
{
	UpdateNiagaraParameters();
}

void ACapturePoint::UpdateNiagaraParameters()
{
	if (!NiagaraEffect) return;

	if (!NiagaraEffect->IsActive())
		NiagaraEffect->Activate();
	
	NiagaraEffect->SetVariableFloat(NiagaraParamProgress,GetNormalizedProgress());
	NiagaraEffect->SetVariableLinearColor(NiagaraParamTeamColor,GetCurrentTeamColor());
}

float ACapturePoint::GetNormalizedProgress() const
{
	return FMath::Abs(CaptureProgress);
}

FLinearColor ACapturePoint::GetCurrentTeamColor() const
{
	switch (CaptureState)
	{
	case ECapturePointState::TeamOneCapturing:
	case ECapturePointState::TeamOneCaptured:
		return TeamOneColor;

	case ECapturePointState::TeamTwoCapturing:
	case ECapturePointState::TeamTwoCaptured:
		return TeamTwoColor;

	case ECapturePointState::Contested:
		return ContestedColor;

	case ECapturePointState::Neutral:
	default:
		return NeutralColor;
	}
}

#if WITH_EDITOR
void ACapturePoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(ACapturePoint, CaptureExtent))
	{
		CaptureZone->SetBoxExtent(CaptureExtent);
	}
}
#endif
