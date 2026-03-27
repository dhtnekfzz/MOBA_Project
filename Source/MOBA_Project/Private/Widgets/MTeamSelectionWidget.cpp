#include "Widgets/MTeamSelectionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMTeamSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SelectButton->OnClicked.AddDynamic(this, &UMTeamSelectionWidget::OnSelectButtonClicked);
}

void UMTeamSelectionWidget::SetSlotID(uint8 NewSlotID)
{
	SlotID = NewSlotID;
}

void UMTeamSelectionWidget::UpdateSlotInfo(const FString& PlayerNickName)
{
	if (InfoText)
		InfoText->SetText(FText::FromString(PlayerNickName));
}

void UMTeamSelectionWidget::OnSelectButtonClicked()
{
	OnMSlotClicked.Broadcast(SlotID);
}
