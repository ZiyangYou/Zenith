// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdateDTT.h"
#include "AIController.h"
#include "EnemyAbstract.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

void UUpdateDTT::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto BlackBoardComp = OwnerComp.GetBlackboardComponent();

	auto AIController = OwnerComp.GetAIOwner();
	auto ControlledPawn =AIController->GetPawn();
	//auto Target = Cast<AActor> (BlackBoardComp->GetValueAsObject("Target"));
	auto Target = BlackBoardComp->GetValueAsVector("LastTargetLocation");
	//UE_LOG(LogTemp, Warning, TEXT("Target location: %s"), *Target.ToString())
	
	float Distance = 0;

	//check if Target location is ZeroVector(location is invalid)
	if (Target.IsZero() == false) 
	{
		Distance = FVector::Dist(Target, ControlledPawn->GetActorLocation());
		//(LogTemp, Warning, TEXT("ControlledPawn location: %s"), *ControlledPawn->GetTargetLocation().ToString())
	}
	else 
	{
		Distance = 10000.0f;
	}

	BlackBoardComp->SetValueAsFloat("DistanceToTarget", Distance);
}