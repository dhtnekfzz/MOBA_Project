// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "MGameplayTags.h"
#include "MOBA_Project.h"
#include "Components/CapsuleComponent.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "GAS/MAbilitySystemComponent.h"
#include "GAS/MAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "GAS/MHeroAttributeSet.h"
#include "Widgets/MVVM/MVVM_Gauge.h"
#include "Widgets/MVVM/MVVM_HeroStats.h"
#include "View/MVVMView.h"
#include "MVVMGameSubsystem.h"
#include "MVVMSubsystem.h"
#include "Character/MPlayerCharacter.h"
#include "DataAssets/StartUpData/DataAsset_PlayerStartUpData.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InventoryItem/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Player/MPlayerState.h"
#include "Widgets/Subsystems/PlayerStatsSubsystem.h"

// Sets default values
AMCharacter::AMCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_SpringArm, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target,ECR_Ignore);

	ASC=CreateDefaultSubobject<UMAbilitySystemComponent>("MAbilitySystemComponent");
	MAttributeSet=CreateDefaultSubobject<UMAttributeSet>("MAttributeSet");

	OverHeadWidgetComponent=CreateDefaultSubobject<UWidgetComponent>("OverHeadWidgetComponent");
	OverHeadWidgetComponent->SetupAttachment(RootComponent);
	OverHeadWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	
	BindGASChangeDelegates();

	PerceptionStimuliSourceComponent=CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Perception Stimuli Source Component");
}

TArray<FMPlayerAbilitySet> AMCharacter::GetAbilitySet()
{
	UDataAsset_PlayerStartUpData* Data=Cast<UDataAsset_PlayerStartUpData>(ASC->GetCharacterStartUpData());
	if (!Data) return TArray<FMPlayerAbilitySet>();
	
	return Data->GetAbilitySet();
}

void AMCharacter::UpgradeAbilityWithInputTag(FGameplayTag& InInputTag)
{
	ASC->Server_UpgradeAbilityWithTag(InInputTag);
}

void AMCharacter::ServerSideInit()
{
	ASC->InitAbilityActorInfo(this, this);
	ASC->InitializeBaseAttributes();
	if (UDataAsset_StartUpDataBase* LoadedData=ASC->GetCharacterStartUpData())
		LoadedData->GiveToAbilitySystemComponent(ASC);

	if (UInventoryComponent* Inv=FindComponentByClass<UInventoryComponent>())
		Inv->InitWithASC(ASC);
	
}

void AMCharacter::ClientSideInit()
{
	ASC->InitAbilityActorInfo(this, this);
}

bool AMCharacter::IsLocallyControlledByPlayer() const
{
	return GetController() && GetController()->IsLocalPlayerController();
}

void AMCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMCharacter, TeamID);
}

void AMCharacter::BeginPlay()
{
	Super::BeginPlay();

	ConfigureOverHeadStatWidget();
	MeshRelativeTransform=GetMesh()->GetRelativeTransform();
	PerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
	
}

void AMCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController && !NewController->IsPlayerController())
		ServerSideInit();
}

void AMCharacter::InitializeUI()
{
	if (!IsLocallyControlled()) return;
	if (!ASC) return;
	
}

void AMCharacter::ConfigureOverHeadStatWidget()
{
	if (!OverHeadWidgetComponent) return;

	if (IsLocallyControlledByPlayer())
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
		return;
	}

	AActor* Character=GetWorld()->GetFirstPlayerController() ? GetWorld()->GetFirstPlayerController()->GetCharacter() : nullptr;

	if (Character)
	{
		if (GetTeamAttitudeTowards(*Character)==ETeamAttitude::Friendly)
		{
			OverHeadWidgetComponent->SetHiddenInGame(true);
			return;
		}
	}
	
	GuageMV=NewObject<UMVVM_Gauge>(this);
	
	GuageMV->SetCurrentValue(ASC->GetNumericAttribute(UMAttributeSet::GetHealthAttribute()));
	GuageMV->SetMaxValue(ASC->GetNumericAttribute(UMAttributeSet::GetMaxHealthAttribute()));
	
	ASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		GuageMV->SetCurrentValue(Data.NewValue);
	});
	ASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		GuageMV->SetMaxValue(Data.NewValue);
	});

	UUserWidget* HeadWidget = OverHeadWidgetComponent->GetWidget();
	if (!HeadWidget) return;
	
	UMVVMView* View=OverHeadWidgetComponent->GetWidget()->GetExtension<UMVVMView>();
	if (View)
		View->SetViewModel(FName("OverHeadVM"), GuageMV);

	OverHeadWidgetComponent->SetHiddenInGame(true);
	GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
	GetWorldTimerManager().SetTimer(HeadStatGaugeVisibilityUpdateTimerHandle, this, &AMCharacter::UpdateHeadStatGaugeVisibility, HeadStatGaugeVisibilityCheckUpdateGap, true);
	
}


void AMCharacter::UpdateHeadStatGaugeVisibility()
{
	APawn* LocalPlayerPawn=UGameplayStatics::GetPlayerPawn(this, 0);
	if (!LocalPlayerPawn) return;

	float DistSquared=FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
	OverHeadWidgetComponent->SetHiddenInGame(DistSquared>HeadStatGaugeVisibilityRangeSquared);
}

void AMCharacter::SetStatGaugeEnabled(bool bIsEnabled)
{
	GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
	
	if (bIsEnabled)
		ConfigureOverHeadStatWidget();
	else
		OverHeadWidgetComponent->SetHiddenInGame(true);
}

// Called every frame
void AMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AMCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void AMCharacter::Server_SendGameplayEventToSelf_Implementation(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}

bool AMCharacter::Server_SendGameplayEventToSelf_Validate(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	return true;
}

void AMCharacter::BindGASChangeDelegates()
{
	if (!ASC) return;

	ASC->RegisterGameplayTagEvent(MGameplayTags::State_Dead).AddUObject(this, &ThisClass::DeathTagUpdated);
	ASC->RegisterGameplayTagEvent(MGameplayTags::State_Stun).AddUObject(this, &ThisClass::StunTageUpdated);
	ASC->RegisterGameplayTagEvent(MGameplayTags::State_Aim).AddUObject(this, &ThisClass::AimTagUpdated);
	
	ASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &ThisClass::MoveSpeedUpdated);
	ASC->GetGameplayAttributeValueChangeDelegate(UMAttributeSet::GetMoveAccelerationAttribute()).AddUObject(this, &ThisClass::MoveSpeedAccelerationUpdated);
}

void AMCharacter::DeathTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount!=0)
		StartDeathSequence();
	else
		Respawn();
		
}

void AMCharacter::StunTageUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (IsDead()) return;

	if (NewCount!=0)
	{
		OnStun();
		PlayAnimMontage(StunMontage);
	}
	else
	{
		OnRecoverFromStun();
		StopAnimMontage(StunMontage);
	}
}

void AMCharacter::AimTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	SetIsAiming(NewCount!=0);
}

void AMCharacter::MoveSpeedUpdated(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed=Data.NewValue;
}

void AMCharacter::MoveSpeedAccelerationUpdated(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxAcceleration=Data.NewValue;
}

void AMCharacter::SetIsAiming(bool bIsAiming)
{
	bUseControllerRotationYaw=bIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement=!bIsAiming;
	OnAimStateChanged(bIsAiming);
}

void AMCharacter::OnAimStateChanged(bool bIsAiming)
{
	//Override in child class
}

void AMCharacter::OnStun()
{
	//Override in child class
}

void AMCharacter::OnRecoverFromStun()
{
	//Override in child class
}

bool AMCharacter::IsDead() const
{
	return ASC->HasMatchingGameplayTag(MGameplayTags::State_Dead);
}

void AMCharacter::RespawnImmediately()
{
	if (HasAuthority())
		ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(MGameplayTags::State_Dead));
}

void AMCharacter::DeathMontageEnded()
{
	if (IsDead())
		SetRagdollEnabled(true);
}

void AMCharacter::SetRagdollEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	}
}

void AMCharacter::PlayDeathAnimation()
{
	if (!DeathMontage) return;

	float MontageDuration=PlayAnimMontage(DeathMontage);
	GetWorldTimerManager().SetTimer(DeathMontageTimerHandle, this, &AMCharacter::DeathMontageEnded, MontageDuration+DeathMontageEndTimeShift);
}

void AMCharacter::StartDeathSequence()
{
	OnDead();

	if (ASC)
		ASC->CancelAbilities();

	PlayDeathAnimation();
	SetStatGaugeEnabled(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetAIPerceptionStimuliSourceEnable(false);
}

void AMCharacter::Respawn()
{
	OnRespawn();
	SetAIPerceptionStimuliSourceEnable(true);
	SetRagdollEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	SetStatGaugeEnabled(true);

	if (HasAuthority() && GetController())
	{
		TWeakObjectPtr<AActor> StartSpot=GetController()->StartSpot;
		if (StartSpot.IsValid())
			SetActorTransform(StartSpot->GetActorTransform());
	}

	if (ASC)
		ASC->GetCharacterStartUpData()->ApplyFullStatEffect(ASC);
	
}

void AMCharacter::OnDead()
{
}

void AMCharacter::OnRespawn()
{
}

void AMCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID=NewTeamID;
}

FGenericTeamId AMCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void AMCharacter::OnRep_TeamID()
{
	//override in child class
}

void AMCharacter::SetAIPerceptionStimuliSourceEnable(bool bIsEnabled)
{
	if (!PerceptionStimuliSourceComponent) return;

	if (bIsEnabled)
		PerceptionStimuliSourceComponent->RegisterWithPerceptionSystem();
	else
		PerceptionStimuliSourceComponent->UnregisterFromPerceptionSystem();
}

