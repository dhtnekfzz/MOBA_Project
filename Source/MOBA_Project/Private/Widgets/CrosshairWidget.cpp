// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CrosshairWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MGameplayTags.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
	OuterReticleImage->SetVisibility(ESlateVisibility::Hidden);
	
	UAbilitySystemComponent* OwnerASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if(OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(MGameplayTags::State_Crosshair).AddUObject(this,&ThisClass::CrosshairTagUpdated);
		OwnerASC->GenericGameplayEventCallbacks.Add(MGameplayTags::Target_Updated).AddUObject(this, &ThisClass::TargetUpdated);
	}

	CachedPlayerController=GetOwningPlayer();

	CrosshairCanvasPanelSlot=Cast<UCanvasPanelSlot>(Slot);
	CrosshairCanvasPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (CrosshairImage->GetVisibility()==ESlateVisibility::Visible)
		UpdateCrosshairPosition();
}

void UCrosshairWidget::CrosshairTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	CrosshairImage->SetVisibility( NewCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden );
	OuterReticleImage->SetVisibility(NewCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UCrosshairWidget::UpdateCrosshairPosition()
{
	if (!CachedPlayerController || ! CrosshairCanvasPanelSlot ) return;

	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
	
	int32 SizeX, SizeY;
	CachedPlayerController->GetViewportSize(SizeX, SizeY);
	
	if (!AimTarget)
	{
		FVector2D ViewportSize = FVector2D{(float)SizeX, (float)SizeY};
		CrosshairCanvasPanelSlot->SetPosition(ViewportSize / 2.f / ViewportScale);
		return;
	}

	FVector2D TargetScreenPosition;
	CachedPlayerController->ProjectWorldLocationToScreen(AimTarget->GetActorLocation(), TargetScreenPosition);
	if (TargetScreenPosition.X > 0 && TargetScreenPosition.X < SizeX && TargetScreenPosition.Y > 0 && TargetScreenPosition.Y < SizeY)
	{
		CrosshairCanvasPanelSlot->SetPosition(TargetScreenPosition / ViewportScale);
	}
}

void UCrosshairWidget::TargetUpdated(const struct FGameplayEventData* EventData)
{
	AimTarget=EventData->Target;
	CrosshairImage->SetColorAndOpacity(AimTarget ? HasTargetColor : NoTargetColor);
	OuterReticleImage->SetColorAndOpacity(AimTarget ? HasTargetColor : NoTargetColor);
}
