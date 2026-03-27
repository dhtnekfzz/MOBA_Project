#include "Widgets/MSessionEntryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMSessionEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SessionBtn->OnClicked.AddDynamic(this, &UMSessionEntryWidget::OnSessionBtnClicked);
}

void UMSessionEntryWidget::InitializeEntry(const FString& Name, const FString& InSessionIdStr)
{
	if (SessionNameText)
		SessionNameText->SetText(FText::FromString(Name));
	
	CachedSessionIdStr = InSessionIdStr;
}

void UMSessionEntryWidget::OnSessionBtnClicked()
{
	OnMSessionEntrySelected.Broadcast(CachedSessionIdStr);
}
