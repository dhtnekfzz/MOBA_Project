// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/MCharacterDisplay.h"
#include "DataAssets/DA_MCharacterDefinition.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"

AMCharacterDisplay::AMCharacterDisplay()
{
	PrimaryActorTick.bCanEverTick = false;

	// 루트 컴포넌트 설정
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	// 캐릭터 메시 표시용 컴포넌트
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
	MeshComponent->SetupAttachment(GetRootComponent());

	// 로비 전용 카메라 — SetViewTarget 시 이 카메라가 활성화됨
	ViewCamera = CreateDefaultSubobject<UCameraComponent>("ViewCamera");
	ViewCamera->SetupAttachment(GetRootComponent());
}


void AMCharacterDisplay::ConfigureWithDefinition(const UDA_MCharacterDefinition* Definition)
{
	if (!Definition) return;

	USkeletalMesh* Mesh = Definition->LoadDisplayMesh();
	if (Mesh)
		MeshComponent->SetSkeletalMesh(Mesh);

	TSubclassOf<UAnimInstance> AnimBP = Definition->LoadDisplayAnimBP();
	if (AnimBP)
	{
		MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		MeshComponent->SetAnimInstanceClass(AnimBP);
	}
}
