// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MGameplayTags.h"
#include "Character/MCharacter.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "GAS/MAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GAS/MAbilitySystemStatics.h"
#include "GAS/MHeroAttributeSet.h"
#include "MTypes/MStructTypes.h"

UMAbilitySystemComponent::UMAbilitySystemComponent()
{

}

void UMAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	
	GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HealthUpdated);
	GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::ManaUpdated);
	GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &ThisClass::ExperienceUpdated);

	GenericConfirmInputID=100;
	GenericCancelInputID=101;
}

void UMAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!CharacterStartUpData) return;
	if (!CharacterStartUpData->GetBaseStatDataTable()) return;
	
	const FRealCurve* ExperienceCurve = CharacterStartUpData->GetExperienceCurve();
	if (ExperienceCurve)
	{
		int MaxLevel = ExperienceCurve->GetNumKeys();
		SetNumericAttributeBase(UMHeroAttributeSet::GetMaxLevelAttribute(), MaxLevel);

		float MaxExp = ExperienceCurve->GetKeyValue(ExperienceCurve->GetLastKeyHandle());
		SetNumericAttributeBase(UMHeroAttributeSet::GetMaxLevelExperienceAttribute(), MaxExp);

		UE_LOG(LogTemp, Warning, TEXT("Max Level is: %d, max experience is: %f"), MaxLevel, MaxExp);
	}

	ExperienceUpdated(FOnAttributeChangeData());
}

bool UMAbilitySystemComponent::IsAtMaxLevel() const
{
	bool bFound;
	float CurrentLevel=GetGameplayAttributeValue(UMHeroAttributeSet::GetLevelAttribute(), bFound);
	float MaxLevel=GetGameplayAttributeValue(UMHeroAttributeSet::GetMaxLevelAttribute(), bFound);
	return CurrentLevel>=MaxLevel;
}

void UMAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;
	
	if (InInputTag == MGameplayTags::InputTag_Confirm)
	{
		AbilityLocalInputPressed(GenericConfirmInputID);
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;
		
		TryActivateAbility(AbilitySpec.Handle);
	}
}

void UMAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid() || !HasMatchingGameplayTag(MGameplayTags::InputTag_Held)) return;

	if (InInputTag == MGameplayTags::InputTag_Cancel)
	{
		AbilityLocalInputPressed(GenericCancelInputID);
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

TObjectPtr<UDataAsset_StartUpDataBase> UMAbilitySystemComponent::GetCharacterStartUpData() const
{
	if (CharacterStartUpData)
		return CharacterStartUpData;
	return nullptr;
}

void UMAbilitySystemComponent::Server_UpgradeAbilityWithTag_Implementation(FGameplayTag AbilityTag)
{
	if (!AbilityTag.IsValid()) return;

	bool bFound = false;
	const float UpgradePoint = GetGameplayAttributeValue(UMHeroAttributeSet::GetUpgradePointAttribute(), bFound);

	if (!bFound || UpgradePoint <= 0.f) return;

	FGameplayAbilitySpec* AbilitySpec = nullptr;
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(AbilityTag))
		{
			AbilitySpec = &Spec;
			break;
		}
	}

	if (!AbilitySpec) return;
	if (UMAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec)) return;

	SetNumericAttributeBase(UMHeroAttributeSet::GetUpgradePointAttribute(),UpgradePoint - 1.f);

	AbilitySpec->Level += 1;
	MarkAbilitySpecDirty(*AbilitySpec);
	
	Client_AbilitySpecLevelUpdated(AbilitySpec->Handle, AbilitySpec->Level);
}

bool UMAbilitySystemComponent::Server_UpgradeAbilityWithTag_Validate(FGameplayTag AbilityTag)
{
	return true;
}

void UMAbilitySystemComponent::Client_AbilitySpecLevelUpdated_Implementation(FGameplayAbilitySpecHandle Handle,
	int32 NewLevel)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
	if (!Spec) return;

	Spec->Level = NewLevel;

	AbilitySpecDirtiedCallbacks.Broadcast(*Spec);
}



void UMAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner()||!GetOwner()->HasAuthority()) return;

	if (ChangeData.NewValue<=0)
	{
		if (!HasMatchingGameplayTag(MGameplayTags::State_Health_Empty))
		{
			AddLooseGameplayTag(MGameplayTags::State_Health_Empty);

			if (UDataAsset_StartUpDataBase* Data = CharacterStartUpData)
				Data->AuthApplyGameplayEffect(this, Data->GetDeathEffect());
		

			FGameplayEventData DeadAbilityEventData;
			if (ChangeData.GEModData)
				DeadAbilityEventData.ContextHandle=ChangeData.GEModData->EffectSpec.GetContext();

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), MGameplayTags::State_Dead, DeadAbilityEventData);
			
		}
	}
	else
		RemoveLooseGameplayTag(MGameplayTags::State_Health_Empty);
	
}

void UMAbilitySystemComponent::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	
}

void UMAbilitySystemComponent::ExperienceUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	if (IsAtMaxLevel()) return;

	if (!CharacterStartUpData) return;

	float CurrentExp = ChangeData.NewValue;

	const FRealCurve* ExperienceCurve = CharacterStartUpData->GetExperienceCurve();
	if (!ExperienceCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find Experience Data!!"));
		return;
	}

	float PrevLevelExp = 0.f;
	float NextLevelExp = 0.f;
	float NewLevel = 1.f;

	for (auto Iter = ExperienceCurve->GetKeyHandleIterator(); Iter; ++Iter)
	{
		float ExperienceToReachLevel = ExperienceCurve->GetKeyValue(*Iter);
		if (CurrentExp < ExperienceToReachLevel)
		{
			NextLevelExp = ExperienceToReachLevel;
			break;
		}
				
		PrevLevelExp = ExperienceToReachLevel;
		NewLevel = Iter.GetIndex() + 1;
	}
	

	float CurrentLevel = GetNumericAttributeBase(UMHeroAttributeSet::GetLevelAttribute());
	float CurrentUpgradePoint = GetNumericAttribute(UMHeroAttributeSet::GetUpgradePointAttribute());

	float LevelUpgraded = NewLevel - CurrentLevel;
	float NewUpgradePoint = CurrentUpgradePoint + LevelUpgraded;

	SetNumericAttributeBase(UMHeroAttributeSet::GetLevelAttribute(), NewLevel);
	SetNumericAttributeBase(UMHeroAttributeSet::GetPrevLevelExperienceAttribute(), PrevLevelExp);
	SetNumericAttributeBase(UMHeroAttributeSet::GetNextLevelExperienceAttribute(), NextLevelExp);
	SetNumericAttributeBase(UMHeroAttributeSet::GetUpgradePointAttribute(), NewUpgradePoint);
}
