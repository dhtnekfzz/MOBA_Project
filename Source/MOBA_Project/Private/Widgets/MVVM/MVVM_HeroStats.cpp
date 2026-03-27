// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MVVM/MVVM_HeroStats.h"
#include "GAS/MAbilitySystemComponent.h"
#include "GAS/MHeroAttributeSet.h"


void UMVVM_HeroStats::UpdateExperience(const FOnAttributeChangeData& Data, UMAbilitySystemComponent* ASC)
{
	bool bFound=false;
	float CurrentExp = ASC->GetGameplayAttributeValue(UMHeroAttributeSet::GetExperienceAttribute(), bFound);
	if (!bFound) return;
	
	float NextLevelExp = ASC->GetGameplayAttributeValue(UMHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound);
	if (!bFound) return;
	
	float PrevLevelExp = ASC->GetGameplayAttributeValue(UMHeroAttributeSet::GetPrevLevelExperienceAttribute(), bFound);
	if (!bFound) return;

	float CurrentLevel = ASC->GetGameplayAttributeValue(UMHeroAttributeSet::GetLevelAttribute(), bFound);
	if (!bFound) return;

	float Progress = CurrentExp - PrevLevelExp;
	float LevelExpAmt = NextLevelExp - PrevLevelExp;
	float Percent=Progress/LevelExpAmt;
	
	if (NextLevelExp==0)
		Percent=1;

	UE_LOG(LogTemp, Warning, TEXT("CurrentExp: %f"),CurrentExp);
	UE_LOG(LogTemp, Warning, TEXT("NextLevelExp: %f"),NextLevelExp);
	UE_LOG(LogTemp, Warning, TEXT("PrevLevelExp: %f"),PrevLevelExp);
	UE_LOG(LogTemp, Warning, TEXT("Percent: %f"),Percent);

	SetLevel(CurrentLevel);
	SetExperiencePercent(Percent);
	
}

void UMVVM_HeroStats::SetAttackDamage(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(AttackDamage, NewValue);
}

void UMVVM_HeroStats::SetArmor(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(Armor, NewValue);
}

void UMVVM_HeroStats::SetIntelligence(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(Intelligence, NewValue);
}

void UMVVM_HeroStats::SetStrength(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(Strength, NewValue);
}

void UMVVM_HeroStats::SetMoveSpeed(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(MoveSpeed, NewValue);
}

void UMVVM_HeroStats::SetCurrentHealth(float NewValue)
{
	if (NewValue < 0.0f) NewValue = 0.0f;
	UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, NewValue);
}

void UMVVM_HeroStats::SetMaxHealth(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, NewValue);
}

void UMVVM_HeroStats::SetCurrentMana(float NewValue)
{
	if (NewValue < 0.0f) NewValue = 0.0f;
	UE_MVVM_SET_PROPERTY_VALUE(CurrentMana, NewValue);
}

void UMVVM_HeroStats::SetMaxMana(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(MaxMana, NewValue);
}

void UMVVM_HeroStats::SetLevel(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(Level, NewValue);
}

void UMVVM_HeroStats::SetExperience(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(Experience, NewValue);
}

void UMVVM_HeroStats::SetPrevLevelExperience(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(PrevLevelExperience, NewValue);
}

void UMVVM_HeroStats::SetNextLevelExperience(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(NextLevelExperience, NewValue);
}

void UMVVM_HeroStats::SetExperiencePercent(float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Percent: %f"),NewValue);

	UE_MVVM_SET_PROPERTY_VALUE(ExperiencePercent, NewValue);
}

void UMVVM_HeroStats::SetUpgradePoint(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(UpgradePoint, NewValue);
}

void UMVVM_HeroStats::SetGold(float NewValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(Gold, NewValue);
}



