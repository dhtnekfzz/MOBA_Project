// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "MGameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AMAIController::AMAIController()
{
	AIPerceptionComponent=CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	SightConfig=CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");

	SightConfig->DetectionByAffiliation.bDetectEnemies=true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies=false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals=false;

	SightConfig->SightRadius=1000.f;
	SightConfig->LoseSightRadius=1200.f;

	SightConfig->LoseSightRadius=1200.f;

	SightConfig->SetMaxAge(5.f);

	SightConfig->PeripheralVisionAngleDegrees=180.f;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::TargetPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::TargetForgotten);
	
}

void AMAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	IGenericTeamAgentInterface* PawnTeamInterface=Cast<IGenericTeamAgentInterface>(InPawn);
	if (!PawnTeamInterface) return;

	SetGenericTeamId(PawnTeamInterface->GetGenericTeamId());
	PawnTeamInterface->SetGenericTeamId(GetGenericTeamId());
	ClearAndDisableAllSenses();
	EnableAllSenses();

	UAbilitySystemComponent* PawnASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn);
	if (!PawnASC) return;

	PawnASC->RegisterGameplayTagEvent(MGameplayTags::State_Dead).AddUObject(this, &AMAIController::PawnDeadTagUpdated);
	PawnASC->RegisterGameplayTagEvent(MGameplayTags::State_Stun).AddUObject(this, &AMAIController::PawnStunTagUpdated);
	
}

void AMAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BehaviorTree);
}

void AMAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!GetCurrentTarget())
			SetCurrentTarget(TargetActor);
	}
	else
	{
		ForgetActorIfDead(TargetActor);
		
		if (GetCurrentTarget() == TargetActor)
		{
			SetCurrentTarget(GetNextPerceivedActor());
		}
	}

}

void AMAIController::TargetForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor) return;

	if (GetCurrentTarget()==ForgottenActor)
		SetCurrentTarget(GetNextPerceivedActor());
}

const UObject* AMAIController::GetCurrentTarget() const
{
	const UBlackboardComponent* BlackboardComponent=GetBlackboardComponent();
	
	if (BlackboardComponent)
		return GetBlackboardComponent()->GetValueAsObject(TargetBlackBoardKeyName);
	return nullptr;
}

void AMAIController::SetCurrentTarget(AActor* NewTarget)
{
	UBlackboardComponent* BlackboardComponent=GetBlackboardComponent();

	if (!BlackboardComponent) return;

	if (NewTarget)
		BlackboardComponent->SetValueAsObject(TargetBlackBoardKeyName, NewTarget);
	else
		BlackboardComponent->ClearValue(TargetBlackBoardKeyName);
}

AActor* AMAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent)
	{
		TArray<AActor*> Actors;
		AIPerceptionComponent->GetPerceivedHostileActors(Actors);

		if (Actors.Num() != 0)
		{
			return Actors[0];
		}
	}

	return nullptr;
}


void AMAIController::ForgetActorIfDead(AActor* ActorToForget)
{
	const UAbilitySystemComponent* ActorASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget);
	if (!ActorASC) return;

	if (!ActorASC->HasMatchingGameplayTag(MGameplayTags::State_Dead)) return;

	for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator Iter=AIPerceptionComponent->GetPerceptualDataIterator(); Iter; ++Iter)
	{
		if (Iter->Key!=ActorToForget) continue;
		
		for (FAIStimulus& Stimuli : Iter->Value.LastSensedStimuli)
			Stimuli.SetStimulusAge(TNumericLimits<float>::Max());
	}
		
}

void AMAIController::ClearAndDisableAllSenses()
{
	AIPerceptionComponent->AgeStimuli(TNumericLimits<float>::Max());

	for (auto SenseConfigIt=AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), false);

	if (GetBlackboardComponent())
		GetBlackboardComponent()->ClearValue(TargetBlackBoardKeyName);
}

void AMAIController::EnableAllSenses()
{
	for (auto SenseConfigIt=AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), true);
}

void AMAIController::PawnDeadTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (Count!=0)
	{
		GetBrainComponent()->StopLogic("Dead");
		ClearAndDisableAllSenses();
		bIsPawnDead=true;
	}
	else
	{
		GetBrainComponent()->StartLogic();
		EnableAllSenses();
		bIsPawnDead=false;
	}
}

void AMAIController::PawnStunTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (bIsPawnDead) return;

	if (Count!=0)
		GetBrainComponent()->StopLogic("Stun");
	else
		GetBrainComponent()->StartLogic();
}
