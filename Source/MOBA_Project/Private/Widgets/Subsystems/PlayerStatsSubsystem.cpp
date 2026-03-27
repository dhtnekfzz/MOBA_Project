// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Subsystems/PlayerStatsSubsystem.h"

#include "Character/MCharacter.h"
#include "Engine/AssetManager.h"
#include "GAS/MAbilitySystemComponent.h"
#include "GAS/MHeroAttributeSet.h"
#include "GAS/MAttributeSet.h"
#include "Types/MVVMViewModelContext.h"
#include "Widgets/MVVM/MVVM_HeroStats.h"
#include "MVVMGameSubsystem.h"
#include "Character/MPlayerCharacter.h"
#include "GAS/MAbilitySystemStatics.h"

class UMVVMGameSubsystem;

void UPlayerStatsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	StatsViewModel=NewObject<UMVVM_HeroStats>(this);
	
	if (UMVVMGameSubsystem* MVVMSubsystem = GetLocalPlayer()->GetGameInstance()->GetSubsystem<UMVVMGameSubsystem>())
	{
		MVVMSubsystem->GetViewModelCollection()->AddViewModelInstance(FMVVMViewModelContext(UMVVM_HeroStats::StaticClass(), FName("StatsVM")), StatsViewModel);
	}
}

void UPlayerStatsSubsystem::InitAbilityViewModels(UMAbilitySystemComponent* ASC, const TArray<FMPlayerAbilitySet>& AbilitySets)
{
	ClearAbilityViewModels();

	for (const auto& AbilitySet : AbilitySets)
	{
		TSubclassOf<UGameplayAbility> AbilityClass = AbilitySet.Ability;
		if (!AbilityClass) continue;

		UGameplayAbility* CDO = AbilityClass.Get()->GetDefaultObject<UGameplayAbility>();
		if (!IsValid(CDO)) continue;
		
		const FAbilityWidgetData* WidgetData = FindWidgetData(AbilityClass);
		if (!WidgetData) continue;

		UTexture2D* LoadedIcon = WidgetData->Icon.LoadSynchronous();

		UMVVM_AbilityEntry* VM = NewObject<UMVVM_AbilityEntry>(this);
		VM->Initialize(ASC, CDO, WidgetData,LoadedIcon);
		AbilityViewModels.Add(VM);
	}
}

void UPlayerStatsSubsystem::InitPreviewAbilityViewModels(const TArray<FAbilityWidgetData*> WidgetDataList)
{
	ClearAbilityViewModels(); 
	for (const auto& Data : WidgetDataList)
	{
		
		float Cooldown = UMAbilitySystemStatics::GetStaticCooldownDurationForAbility(Data->AbilityClass.GetDefaultObject());
		float Cost = UMAbilitySystemStatics::GetStaticCostForAbility(Data->AbilityClass.GetDefaultObject());


		UMVVM_AbilityEntry* VM = NewObject<UMVVM_AbilityEntry>(this);
		
		VM->SetIcon(Data->Icon.LoadSynchronous());
		VM->SetCooldownDuration(Cooldown);
		VM->SetManaCost(Cost);
		VM->SetAbilityNameText(Data->AbilityName);
		VM->SetAbilityDescriptionText(Data->Description);
		
		AbilityViewModels.Add(VM);
	}
}


void UPlayerStatsSubsystem::OnPawnPossessed(APawn* NewPawn)
{
	UnbindFromASC();
	if (AMPlayerCharacter* Character = Cast<AMPlayerCharacter>(NewPawn))
	{
		if (CachedASC=Cast<UMAbilitySystemComponent>(Character->GetAbilitySystemComponent()))
		{
			BindToASC(CachedASC);
			TArray<FMPlayerAbilitySet> AbilitySets=Character->GetAbilitySet();
			if (AbilitySets.IsEmpty()) return;
			InitAbilityViewModels(CachedASC, AbilitySets);
		}
	}
}

void UPlayerStatsSubsystem::OnPawnUnPossessed()
{
	UnbindFromASC();
}

void UPlayerStatsSubsystem::BindToASC(UMAbilitySystemComponent* ASC)
{
	auto BindAttributeToViewModel = [&](auto ViewModel, const FGameplayAttribute& Attribute, auto SetterFunc)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda(
			[this,ViewModel,SetterFunc](const FOnAttributeChangeData& Data)
			{
				(ViewModel->*SetterFunc)(Data.NewValue);
			});

		(ViewModel->*SetterFunc)(ASC->GetNumericAttribute(Attribute));
	};

	BindAttributeToViewModel(StatsViewModel, UMAttributeSet::GetHealthAttribute(), &UMVVM_HeroStats::SetCurrentHealth);
	BindAttributeToViewModel(StatsViewModel, UMAttributeSet::GetMaxHealthAttribute(), &UMVVM_HeroStats::SetMaxHealth);
	BindAttributeToViewModel(StatsViewModel, UMAttributeSet::GetManaAttribute(), &UMVVM_HeroStats::SetCurrentMana);
	BindAttributeToViewModel(StatsViewModel, UMAttributeSet::GetMaxManaAttribute(), &UMVVM_HeroStats::SetMaxMana);
	BindAttributeToViewModel(StatsViewModel, UMAttributeSet::GetAttackDamageAttribute(), &UMVVM_HeroStats::SetAttackDamage);
	BindAttributeToViewModel(StatsViewModel, UMAttributeSet::GetArmorAttribute(), &UMVVM_HeroStats::SetArmor);
	BindAttributeToViewModel(StatsViewModel, UMAttributeSet::GetMoveSpeedAttribute(), &UMVVM_HeroStats::SetMoveSpeed);
	
	BindAttributeToViewModel(StatsViewModel, UMHeroAttributeSet::GetIntelligenceAttribute(), &UMVVM_HeroStats::SetIntelligence);
	BindAttributeToViewModel(StatsViewModel, UMHeroAttributeSet::GetStrengthAttribute(), &UMVVM_HeroStats::SetStrength);
	BindAttributeToViewModel(StatsViewModel, UMHeroAttributeSet::GetLevelAttribute(), &UMVVM_HeroStats::SetLevel);
	BindAttributeToViewModel(StatsViewModel, UMHeroAttributeSet::GetExperienceAttribute(), &UMVVM_HeroStats::SetExperience);
	BindAttributeToViewModel(StatsViewModel, UMHeroAttributeSet::GetUpgradePointAttribute(), &UMVVM_HeroStats::SetUpgradePoint);
	BindAttributeToViewModel(StatsViewModel, UMHeroAttributeSet::GetGoldAttribute(), &UMVVM_HeroStats::SetGold);

	ASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetExperienceAttribute()).AddUObject(StatsViewModel, &UMVVM_HeroStats::UpdateExperience, ASC);
	ASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetPrevLevelExperienceAttribute()).AddUObject(StatsViewModel, &UMVVM_HeroStats::UpdateExperience, ASC);
	ASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(StatsViewModel, &UMVVM_HeroStats::UpdateExperience, ASC);
	ASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetLevelAttribute()).AddUObject(StatsViewModel, &UMVVM_HeroStats::UpdateExperience, ASC);

	
}

void UPlayerStatsSubsystem::UnbindFromASC()
{
	if (!CachedASC) return;

	ClearAbilityViewModels();
	
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetHealthAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetMaxHealthAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetManaAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetMaxManaAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetAttackDamageAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetArmorAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetMoveSpeedAttribute()).RemoveAll(this);
	
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetIntelligenceAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetStrengthAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetUpgradePointAttribute()).RemoveAll(this);

	CachedASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetExperienceAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetPrevLevelExperienceAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetNextLevelExperienceAttribute()).RemoveAll(this);
	CachedASC->GetGameplayAttributeValueChangeDelegate(UMHeroAttributeSet::GetLevelAttribute()).RemoveAll(this);


}

void UPlayerStatsSubsystem::ClearAbilityViewModels()
{
	for (UMVVM_AbilityEntry* VM : AbilityViewModels)
		if (VM) VM->DeInitialize();

	if (AbilityViewModels.IsEmpty()) return;
	
	AbilityViewModels.Empty();
}

const FAbilityWidgetData* UPlayerStatsSubsystem::FindWidgetData(const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
	if (!AbilityDataTable) return nullptr;

	for (auto& [RowName, RowPtr] : AbilityDataTable->GetRowMap())
	{
		const FAbilityWidgetData* Data = reinterpret_cast<const FAbilityWidgetData*>(RowPtr);
		if (Data && Data->AbilityClass == AbilityClass)
			return Data;
	}
	return nullptr;
}
