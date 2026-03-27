// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MRespawnWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MGameplayTags.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMRespawnWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	UAbilitySystemComponent* OwnerASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if(OwnerASC)
		OwnerASC->RegisterGameplayTagEvent(MGameplayTags::State_Dead).AddUObject(this,&ThisClass::DeadTagUpdated);
	
	
}

void UMRespawnWidget::DeadTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount !=0)
	{
		SetVisibility(ESlateVisibility::Visible);
		TimeRemaining=RespawnTime;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::StartCountdown, 1.f, true,0);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

		RespawnTimeText->SetText(FText::AsNumber(0));
		RespawnImage->GetDynamicMaterial()->SetScalarParameterValue(ParamName, 0.f);
		FillGlow->GetDynamicMaterial()->SetScalarParameterValue(ParamName, 0.f);

		FTimerHandle HideTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, [this]()
		{
			if (IsValid(this)) 
			{
				SetVisibility(ESlateVisibility::Hidden);
			}
		}, 0.1f, false);
	}
}

void UMRespawnWidget::StartCountdown()
{
	UpdateTime(TimeRemaining);

}

void UMRespawnWidget::UpdateTime(float InTimeRemaining)
{
	float Time = FMath::Max(TimeRemaining, 0.f);
	RespawnTimeText->SetText(FText::AsNumber(Time));
	
	RespawnImage->GetDynamicMaterial()->SetScalarParameterValue(ParamName, Time/RespawnTime);
	FillGlow->GetDynamicMaterial()->SetScalarParameterValue(ParamName, Time/RespawnTime);
	TimeRemaining--;
}

