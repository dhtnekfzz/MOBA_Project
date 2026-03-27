// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "GAS/Abilities/MGameplayAbility.h"
#include "MStructTypes.generated.h"

class UDataAsset_PlayerStartUpData;
class UDataAsset_ShopItem;
class UDA_MCharacterDefinition;

USTRUCT(BlueprintType)
struct FMInputActionConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(Category="Input"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAction=nullptr;

	bool IsValid() const { return InputTag.IsValid()&&InputAction; }
};

USTRUCT(BlueprintType)
struct FMPlayerAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Category="Ability"))
	FGameplayTag AbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UMGameplayAbility> Ability;

	bool IsValid() const { return AbilityTag.IsValid() && Ability;}
};

USTRUCT(BlueprintType)
struct FMHeroBaseStats : public FTableRowBase
{
	GENERATED_BODY()
public:
	FMHeroBaseStats();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;

	UPROPERTY(EditAnywhere)
	float Strength;

	UPROPERTY(EditAnywhere)
	float Intelligence;
	
	UPROPERTY(EditAnywhere)
	float StrengthGrowthRate;

	UPROPERTY(EditAnywhere)
	float IntelligenceGrowthRate;

	UPROPERTY(EditAnywhere)
	float BaseMaxHealth;

	UPROPERTY(EditAnywhere)
	float BaseMaxMana;

	UPROPERTY(EditAnywhere)
	float BaseAttackDamage;

	UPROPERTY(EditAnywhere)
	float BaseArmor;

	UPROPERTY(EditAnywhere)
	float BaseMoveSpeed;

	UPROPERTY(EditAnywhere)
	int32 UpgradePoints=0;;
};

USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText AbilityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	
};

USTRUCT(BlueprintType)
struct FMInventoryItemHandle
{
	GENERATED_BODY()

public:
	FMInventoryItemHandle();

	static FMInventoryItemHandle InvalidHandle();
	static FMInventoryItemHandle CreateHandle();

	bool IsValid() const;
	uint32 GetHandleId() const { return HandleId; }

private:
	explicit FMInventoryItemHandle(uint32 InHandleId);

	UPROPERTY()
	uint32 HandleId=0;

	static uint32 GenerateNextId();
	static uint32 GetInvalidId() { return 0; }
};

bool operator==(const FMInventoryItemHandle& A, const FMInventoryItemHandle& B);
uint32 GetTypeHash(const FMInventoryItemHandle& Key);

USTRUCT(BlueprintType)
struct FItemCollection
{
	GENERATED_BODY();

public:
	FItemCollection();
	FItemCollection(const TArray<const UDataAsset_ShopItem*>& InItems);

	void AddItem(const UDataAsset_ShopItem* NewItem, bool bAddUnique=false);
	bool Contains(const UDataAsset_ShopItem* Item) const;
	const TArray<const UDataAsset_ShopItem*>& GetItems() const;

private:
	TArray<const UDataAsset_ShopItem*> Items;
	
};


USTRUCT()
struct FPlayerSelection
{
	GENERATED_BODY()
	
public:
	FPlayerSelection();
	FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState);

	FORCEINLINE void SetSlot(uint8 NewSlot) { Slot = NewSlot; }
	FORCEINLINE uint8 GetPlayerSlot() const { return Slot; }
	FORCEINLINE FUniqueNetIdRepl GetPLayerUniqueId() const { return PlayerUniqueId; }
	FORCEINLINE FString GetPlayerNickName() const { return PlayerNickName; }
	FORCEINLINE const UDA_MCharacterDefinition* GetCharacterDefinition() const { return CharacterDefinition; }
	FORCEINLINE void SetCharacterDefinition(const UDA_MCharacterDefinition* NewCharacterDefinition) { CharacterDefinition = NewCharacterDefinition; }

	bool IsForPlayer(const APlayerState* PlayerState) const;
	bool IsValid() const;

	static uint8 GetInvalidSlot();
	
private:
	UPROPERTY()
	uint8 Slot;

	UPROPERTY()
	FUniqueNetIdRepl PlayerUniqueId;

	UPROPERTY() 
	FString PlayerNickName;

	UPROPERTY()
	const UDA_MCharacterDefinition* CharacterDefinition;
};
