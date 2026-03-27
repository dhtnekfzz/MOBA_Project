// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_InventorySlot.generated.h"

class UInventoryItem;
/**
 * 
 */
UCLASS()
class MOBA_PROJECT_API UMVVM_InventorySlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	void BindToItem(UInventoryItem* Item);   
	void UnbindAndClear();                
	
	void StartCooldownTick();
	void NotifyCooldownStarted(float Duration, float TimeRemaining);

	void SetIcon(UTexture2D* NewIcon);
	void SetStackCount(int32 NewValue);
	void SetCooldownRemaining(float NewValue);
	void SetCooldownDuration(float NewValue);
	void SetManaCost(float NewValue);
	void SetbCanCast(bool NewValue);
	void SetbIsEmpty(bool NewValue);

	UTexture2D* GetIcon() const { return Icon; }
	int32 GetStackCount() const { return StackCount; }
	float GetCooldownRemaining() const { return CooldownRemaining; }
	float GetCooldownDuration() const { return CooldownDuration; }
	float GetManaCost() const { return ManaCost; }
	bool GetbCanCast() const { return bCanCast; }
	bool GetbIsEmpty() const { return bIsEmpty; }

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	int32 StackCount = 0;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	float CooldownRemaining = 0.f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	float CooldownDuration = 0.f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	float ManaCost = 0.f;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	bool bCanCast = false;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess=true))
	bool bIsEmpty = true;

	void OnCanCastUpdated(bool bNew);
	void TickCooldown();
	void FinishCooldown();

	UPROPERTY()
	TObjectPtr<UInventoryItem> CachedItem;

	FTimerHandle CooldownTickTimer;
	
	static constexpr float TickInterval = 0.1f;
};
