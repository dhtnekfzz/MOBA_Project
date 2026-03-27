// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityListView.h"

#include "Widgets/MVVM/MVVM_AbilityEntry.h"

void UAbilityListView::ConfigureWithViewModels(const TArray<UMVVM_AbilityEntry*>& ViewModels)
{
	ClearListItems();
	for (UMVVM_AbilityEntry* VM : ViewModels)
		if (VM) AddItem(VM);
}
