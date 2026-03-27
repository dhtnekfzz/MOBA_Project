#include "Widgets/MWaitingWidget.h"
#include "Components/TextBlock.h"

void UMWaitingWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMWaitingWidget::SetWaitInfo(const FText& WaitInfo, bool bAllowCancel)
{
	if (WaitInfoText)
		WaitInfoText->SetText(WaitInfo);

	if (CancelBtn)
		CancelBtn->SetVisibility(bAllowCancel ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

FOnButtonClickedEvent& UMWaitingWidget::ClearAndGetCancelClickedEvent()
{
	CancelBtn->OnClicked.Clear();
	return CancelBtn->OnClicked;
}
