// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MItemTreeWidget.h"
#include "Widgets/MSplineWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UMItemTreeWidget::DrawFromNode(const ITreeNodeInterface* NodeInterface)
{
	if (!NodeInterface) return;

	if (CurrentCenterItem == NodeInterface->GetItemObject()) return;

	ClearTree();
	CurrentCenterItem = NodeInterface->GetItemObject();

	UCanvasPanelSlot* CenterSlot   = nullptr;
	UUserWidget*      CenterWidget = CreateWidgetForNode(NodeInterface, CenterSlot);

	float NextLeafXPos = 0.f;
	TArray<UCanvasPanelSlot*> LowerSlots;
	DrawStream(false, NodeInterface, CenterWidget, CenterSlot, 0, NextLeafXPos, LowerSlots);

	float LowerXMax   = NextLeafXPos - NodeSize.X - NodeGap.X;
	float LowerOffset = -(LowerXMax / 2.f);
	for (UCanvasPanelSlot* LowerSlot : LowerSlots)
		LowerSlot->SetPosition(LowerSlot->GetPosition() + FVector2D(LowerOffset, 0.f));

	NextLeafXPos = 0.f;
	TArray<UCanvasPanelSlot*> UpperSlots;
	DrawStream(true, NodeInterface, CenterWidget, CenterSlot, 0, NextLeafXPos, UpperSlots);

	float UpperXMax   = NextLeafXPos - NodeSize.X - NodeGap.X;
	float UpperOffset = -(UpperXMax / 2.f);
	for (UCanvasPanelSlot* UpperSlot : UpperSlots)
		UpperSlot->SetPosition(UpperSlot->GetPosition() + FVector2D(UpperOffset, 0.f));

	CenterSlot->SetPosition(FVector2D::ZeroVector);
}

void UMItemTreeWidget::DrawStream(
	bool                       bUpperStream,
	const ITreeNodeInterface*  StartingNodeInterface,
	UUserWidget*               StartingNodeWidget,
	UCanvasPanelSlot*          StartingNodeSlot,
	int32                      StartingNodeDepth,
	float&                     NextLeafXPosition,
	TArray<UCanvasPanelSlot*>& OutStreamSlots)
{
	const TArray<const ITreeNodeInterface*> Children =bUpperStream ? StartingNodeInterface->GetInputs(): StartingNodeInterface->GetOuputs();

	const float YPos = (NodeSize.Y + NodeGap.Y)* static_cast<float>(StartingNodeDepth)* (bUpperStream ? -1.f : 1.f);

	if (Children.IsEmpty())
	{
		StartingNodeSlot->SetPosition(FVector2D(NextLeafXPosition, YPos));
		NextLeafXPosition += NodeSize.X + NodeGap.X;
		return;
	}

	float ChildXPosSum = 0.f;

	for (const ITreeNodeInterface* Child : Children)
	{
		UCanvasPanelSlot* ChildSlot   = nullptr;
		UUserWidget*      ChildWidget = CreateWidgetForNode(Child, ChildSlot);

		OutStreamSlots.Add(ChildSlot);

		if (bUpperStream)
			CreateConnection(ChildWidget, StartingNodeWidget);
		else
			CreateConnection(StartingNodeWidget, ChildWidget);

		DrawStream(bUpperStream, Child, ChildWidget, ChildSlot, StartingNodeDepth + 1, NextLeafXPosition, OutStreamSlots);

		ChildXPosSum += ChildSlot->GetPosition().X;
	}

	const float CenterX = ChildXPosSum / static_cast<float>(Children.Num());
	StartingNodeSlot->SetPosition(FVector2D(CenterX, YPos));
}

void UMItemTreeWidget::ClearTree()
{
	if (RootPanel) RootPanel->ClearChildren();
	CurrentCenterItem = nullptr;
}


UUserWidget* UMItemTreeWidget::CreateWidgetForNode(const ITreeNodeInterface* Node,UCanvasPanelSlot*& OutCanvasSlot)
{
	if (!Node || !RootPanel) return nullptr;

	UUserWidget* NodeWidget = Node->GetWidget();
	if (!NodeWidget) return nullptr;

	OutCanvasSlot = RootPanel->AddChildToCanvas(NodeWidget);
	if (OutCanvasSlot)
	{
		OutCanvasSlot->SetSize(NodeSize);
		OutCanvasSlot->SetAnchors(FAnchors(0.5f));    
		OutCanvasSlot->SetAlignment(FVector2D(0.5f));  
		OutCanvasSlot->SetZOrder(1);                   
	}

	return NodeWidget;
}

void UMItemTreeWidget::CreateConnection(const UUserWidget* From, UUserWidget* To)
{
	if (!From || !To || !RootPanel) return;

	UMSplineWidget* Spline = CreateWidget<UMSplineWidget>(GetOwningPlayer());
	if (!Spline) return;

	UCanvasPanelSlot* SplineSlot = RootPanel->AddChildToCanvas(Spline);
	if (SplineSlot)
	{
		SplineSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		SplineSlot->SetOffsets(FMargin(0.f));
		SplineSlot->SetAlignment(FVector2D(0.f));
		SplineSlot->SetZOrder(0); 
	}

	Spline->SetupSpline(
		From, To,
		SourcePortLocalPos,
		DestinationPortLocalPos,
		SourcePortDirection,
		DestinationPortDirection);

	Spline->SetSplineStyle(ConnectionColor, ConnectionThickness);
}
