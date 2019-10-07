// Fill out your copyright notice in the Description page of Project Settings.

#include "UpdateRandLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyController.h"

EBTNodeResult::Type UUpdateRandLocation::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{

	auto AIController = Cast<AEnemyController>(OwnerComp.GetAIOwner());

	FVector LastPlayerLocation = AIController->BlackboardComp->GetValueAsVector(TEXT("LastTargetLocation"));

	AIController->SetRandomlocation(LastPlayerLocation);

	return EBTNodeResult::Succeeded;
}
