#include "Widgets/MPlayerTeamLayoutWidget.h"
#include "Widgets/MPlayerTeamSlotWidget.h"
#include "Network/MNetStatics.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

void UMPlayerTeamLayoutWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (!PlayerTeamSlotWidgetClass) return;

	TeamOneLayoutBox->ClearChildren();
	TeamTwoLayoutBox->ClearChildren();

	const int32 TotalSlots = UMNetStatics::GetPlayerCountPerTeam() * 2;
	for (int32 i = 0; i < TotalSlots; ++i)
	{
		UMPlayerTeamSlotWidget* SlotWidget = CreateWidget<UMPlayerTeamSlotWidget>(GetOwningPlayer(), PlayerTeamSlotWidgetClass);
		TeamSlotWidgets.Add(SlotWidget);

		UHorizontalBox* TargetBox = (i < UMNetStatics::GetPlayerCountPerTeam()) ? TeamOneLayoutBox : TeamTwoLayoutBox;

		if (UHorizontalBoxSlot* BoxSlot = TargetBox->AddChildToHorizontalBox(SlotWidget))
			BoxSlot->SetPadding(FMargin{SlotMargin});
	}
}


void UMPlayerTeamLayoutWidget::UpdatePlayerSelection(const TArray<FPlayerSelection>& PlayerSelections)
{
	for (UMPlayerTeamSlotWidget* SlotWidget : TeamSlotWidgets)
		SlotWidget->UpdateSlot("", nullptr);

	for (const FPlayerSelection& Sel : PlayerSelections)
	{
		if (!Sel.IsValid()) continue;

		const uint8 SlotIdx = Sel.GetPlayerSlot();
		if (TeamSlotWidgets.IsValidIndex(SlotIdx))
			TeamSlotWidgets[SlotIdx]->UpdateSlot(Sel.GetPlayerNickName(), Sel.GetCharacterDefinition());
	}
}
