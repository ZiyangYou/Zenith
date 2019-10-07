// Fill out your copyright notice in the Description page of Project Settings.

#include "CubbyController.h"
#include "CubbyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyAbstract.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyPlayerController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

ACubbyController::ACubbyController(const FObjectInitializer & ObjectInitializer)
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));


}

void ACubbyController::SetToFloatingMode()
{
	BlackboardComp->SetValueAsEnum(CubbyState, uint8(CubbyState::FloatingNearPlayer));
	OnChangeToFloatingMode.Broadcast();
}

void ACubbyController::SetToGrapplingMode(AActor * Target)
{
	BlackboardComp->SetValueAsEnum(CubbyState, uint8(CubbyState::Grappling));

	FVector TargetLocation = Target->GetActorLocation();

	BlackboardComp->SetValueAsVector(GrappleTargetLocation, TargetLocation);
	BlackboardComp->SetValueAsObject(this->Target, Target);

	OnChangeToGrappleMode.Broadcast();
}

void ACubbyController::SetToComingBackMode()
{
	BlackboardComp->SetValueAsEnum(CubbyState, uint8(CubbyState::ComingBack));

	OnChangeToComeBackMode.Broadcast();
}

void ACubbyController::Possess(APawn * pawn)
{
	Super::Possess(pawn);

	SetUpBT(pawn);

}

void ACubbyController::BeginPlay()
{
	Super::BeginPlay();

	//BlackboardComp->SetValueAsObject(Player, Cast<AActor>(ControlledPawn->PlayerCharacter));

	//SetToFloatingMode();
}

void ACubbyController::SetUpBT(APawn* Pawn)
{
	ControlledPawn = Cast<ACubbyBase>(Pawn);

	if (!ControlledPawn)return;
	if (!ControlledPawn->IsValidLowLevel())return;

	if (ControlledPawn && BaseBehaviorTree)
	{
		BlackboardComp->InitializeBlackboard(*(BaseBehaviorTree->BlackboardAsset));
		BehaviorTreeComp->StartTree(*(BaseBehaviorTree));
	}
}

void ACubbyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}