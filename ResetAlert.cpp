// Fill out your copyright notice in the Description page of Project Settings.

#include "ResetAlert.h"
#include "AIController.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UResetAlert::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{

	auto AIController = Cast<AEnemyController>(OwnerComp.GetAIOwner());

	AIController->BlackboardComp->SetValueAsBool(TEXT("IsAlert"), false);

	AIController->SetToPatrollMode();

	return EBTNodeResult::Succeeded;
}
