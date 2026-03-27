// Fill out your copyright notice in the Description page of Project Settings.


#include "MGameplayTags.h"

namespace MGameplayTags
{
	/* Input Tags */
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "InputTag.Jump");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ctrl, "InputTag.Ctrl");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Shop, "InputTag.Shop");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_GameplayMenu, "InputTag.GameplayMenu");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Shift, "InputTag.Shift");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Held, "InputTag.Held");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Confirm, "InputTag.Confirm");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Cancel, "InputTag.Cancel");


	/* State Tags */
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_Stun, "State_Stun");
	UE_DEFINE_GAMEPLAY_TAG(State_Health_Empty, "State.Health.Empty");
	UE_DEFINE_GAMEPLAY_TAG(State_Aim, "State.Aim");
	UE_DEFINE_GAMEPLAY_TAG(State_Crosshair, "State.Crosshair");

	/* Gameplay Ability Tags */
	UE_DEFINE_GAMEPLAY_TAG(Ability_BasicAttack, "Ability.BasicAttack");
	UE_DEFINE_GAMEPLAY_TAG(Ability_BasicAttack_Pressed, "Ability.BasicAttack.Pressed");
	UE_DEFINE_GAMEPLAY_TAG(Ability_BasicAttack_Released, "Ability.BasicAttack.Released");
	UE_DEFINE_GAMEPLAY_TAG(Ability_One, "Ability.One");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Two, "Ability.Two");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Three, "Ability.Three");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Four, "Ability.Four");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Combo, "Ability.Combo");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Combo_Damage, "Ability.Combo.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dash, "Ability.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dash_Start, "Ability.Dash.Start");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Passive_Launch_Activate, "Ability.Passive.Launch.Activate");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Shoot, "Ability.Shoot");
	UE_DEFINE_GAMEPLAY_TAG(Ability_AOE, "Ability.AOE");

	/* Role Tags */
	UE_DEFINE_GAMEPLAY_TAG(Role_Hero, "Role.Hero");

	/* Attr Tags */
	UE_DEFINE_GAMEPLAY_TAG(Attr_Experience, "Attr.Experience");
	UE_DEFINE_GAMEPLAY_TAG(Attr_Gold, "Attr.Gold");

	UE_DEFINE_GAMEPLAY_TAG(Target_Updated, "Target.Updated");
}