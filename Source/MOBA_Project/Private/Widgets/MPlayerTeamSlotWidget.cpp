#include "Widgets/MPlayerTeamSlotWidget.h"
#include "DataAssets/DA_MCharacterDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMPlayerTeamSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (PlayerCharacterIcon)
		 PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(EmptyParamName, 1.f);
	
	CachedCharacterName = "";
}


void UMPlayerTeamSlotWidget::UpdateSlot(const FString& PlayerName, const UDA_MCharacterDefinition* CharacterDefinition)
{
	CachedPlayerName = PlayerName;
	
	if (CharacterDefinition)
	{
		PlayerCharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(CharacterIconParamName, CharacterDefinition->LoadIcon());
		PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(EmptyParamName, 0.f);
		
		CachedCharacterName = CharacterDefinition->GetCharacterDisplayName();
	}
	else
	{
		PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(EmptyParamName, 1.f);
		CachedCharacterName = "";
	}

	RefreshNameText();
}

void UMPlayerTeamSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	if (NameText)
		NameText->SetText(FText::FromString(CachedCharacterName));
	if (HoverAnim)
		PlayAnimationForward(HoverAnim);
}

void UMPlayerTeamSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	RefreshNameText();
	if (HoverAnim)
		PlayAnimationReverse(HoverAnim);
}

void UMPlayerTeamSlotWidget::RefreshNameText()
{
	if (!NameText) return;
	NameText->SetText(FText::FromString(IsHovered() ? CachedCharacterName : CachedPlayerName));
}
