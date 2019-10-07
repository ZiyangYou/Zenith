// Fill out your copyright notice in the Description page of Project Settings.

#include "RangeAttack.h"
#include "EnemyController.h"

EBTNodeResult::Type URangeAttack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{

	//UE_LOG(LogTemp, Warning, TEXT("Attacking!"))
	bNotifyTick = true;

	AIController = Cast<AEnemyController>(OwnerComp.GetAIOwner());

	AIController->RangeAttack();

	return EBTNodeResult::InProgress;
}

void URangeAttack::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	//auto AIController = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if (!AIController->IsStillRangeAttacking())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

