// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Framework/CapturePoint.h"
#include "CapturePointSubsystem.generated.h"

class UMVVM_CapturePoint;

UCLASS()
class MOBA_PROJECT_API UCapturePointSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	void BindToCapturePoint();
	void UnbindFromCapturePoint();

	UMVVM_CapturePoint* GetViewModel() const { return ViewModel; }

private:
	UPROPERTY()
	TObjectPtr<UMVVM_CapturePoint> ViewModel;

	UPROPERTY()
	TObjectPtr<ACapturePoint> CachedCapturePoint;
	
	void OnCaptureStateChanged(int32 TeamOneCount, int32 TeamTwoCount);
	
	FTimerHandle UpdateTimerHandle;
	void OnUpdateTick();

	
};
