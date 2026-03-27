// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MActivatableWidget.h"
#include "MRespawnWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMRespawnWidget : public UMActivatableWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	void DeadTagUpdated(const FGameplayTag Tag, int32 NewCount);
	
	void StartCountdown();
	void UpdateTime(float InTimeRemaining);

private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* RespawnTimeText;   

	UPROPERTY(meta=(BindWidget))
	class UImage* RespawnImage;  

	UPROPERTY(meta=(BindWidget))
	class UImage* FillGlow;

	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName ParamName="AnimateArcFill";

	UPROPERTY(EditDefaultsOnly, Category="TIme")
	float RespawnTime = 5.f;

	int TimeRemaining=0;

	FTimerHandle TimerHandle;
	
};
