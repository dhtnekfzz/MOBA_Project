// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Subsystems/CapturePointSubsystem.h"
#include "Widgets/MVVM/MVVM_CapturePoint.h"
#include "MVVMGameSubsystem.h"
#include "Types/MVVMViewModelContext.h"
#include "EngineUtils.h"

void UCapturePointSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ViewModel = NewObject<UMVVM_CapturePoint>(this);

	UMVVMGameSubsystem* MVVMSub = GetLocalPlayer()->GetGameInstance()->GetSubsystem<UMVVMGameSubsystem>();
	if (!MVVMSub) return;

	MVVMSub->GetViewModelCollection()->AddViewModelInstance(FMVVMViewModelContext(UMVVM_CapturePoint::StaticClass(), FName("CapturePointVM")),ViewModel);
}

void UCapturePointSubsystem::Deinitialize()
{
	UnbindFromCapturePoint();
	Super::Deinitialize();
}

void UCapturePointSubsystem::BindToCapturePoint()
{
	UnbindFromCapturePoint();

	UWorld* World = GetLocalPlayer()->GetWorld();
	if (!World) return;

	for (TActorIterator<ACapturePoint> It(World); It; ++It)
	{
		CachedCapturePoint = *It;
		break;
	}

	if (!CachedCapturePoint) return;

	CachedCapturePoint->OnCaptureStateChanged.AddUObject(this, &UCapturePointSubsystem::OnCaptureStateChanged);

	World->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UCapturePointSubsystem::OnUpdateTick,0.1f, /*bLoop=*/true);

	OnUpdateTick();
}

void UCapturePointSubsystem::UnbindFromCapturePoint()
{
	if (CachedCapturePoint)
	{
		CachedCapturePoint->OnCaptureStateChanged.RemoveAll(this);
		CachedCapturePoint = nullptr;
	}

	if (UWorld* World = GetLocalPlayer()->GetWorld())
		World->GetTimerManager().ClearTimer(UpdateTimerHandle);
}

void UCapturePointSubsystem::OnCaptureStateChanged(int32 TeamOneCount, int32 TeamTwoCount)
{
	OnUpdateTick();
}


void UCapturePointSubsystem::OnUpdateTick()
{
	if (!CachedCapturePoint || !ViewModel) return;

	ViewModel->UpdateFromCapturePoint(
		CachedCapturePoint->GetCaptureProgress(),
		CachedCapturePoint->GetCaptureState(),
		CachedCapturePoint->GetVictoryTimeRemaining(),
		CachedCapturePoint->GetTeamOneColor(),
		CachedCapturePoint->GetTeamTwoColor());
}
