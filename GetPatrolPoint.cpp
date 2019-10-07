// Fill out your copyright notice in the Description page of Project Settings.


#include "GetPatrolPoint.h"
#include "AIController.h"
#include "PatrolRoute.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UGetPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	//get controlled pawn
	auto AIController = OwnerComp.GetAIOwner();
	auto ControlledPawn = AIController->GetPawn();

	auto PatrolRoute = ControlledPawn->FindComponentByClass<UPatrolRoute>();

	if (!ensure(PatrolRoute)) { return EBTNodeResult::Failed; }

	//get the patrol points
	//auto PatrollingGuard = Cast<ATestProjectCharacter>(ControlledPawn);
	auto PatrolPoints = PatrolRoute->GetPatrolPpoints();
	if (PatrolPoints.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	//get blackboard component
	auto BlackboardComp = OwnerComp.GetBlackboardComponent();

	//set next waypoint
	auto Index = BlackboardComp->GetValueAsInt(IndexPatrolPoint.SelectedKeyName);
	BlackboardComp->SetValueAsObject(PatrolPoint.SelectedKeyName, PatrolPoints[Index]);

	//UE_LOG(LogTemp, Warning, TEXT("waypoint target is: %s"), *PatrolPoints[Index]->GetName());
	//Cycle the index
	auto NextIndex = (Index + 1) % PatrolPoints.Num();
	BlackboardComp->SetValueAsInt(IndexPatrolPoint.SelectedKeyName, NextIndex);




	return EBTNodeResult::Succeeded;

}
