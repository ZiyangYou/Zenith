// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeAttack.h"
#include "EnemyController.h"


EBTNodeResult::Type UMeleeAttack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	
	bNotifyTick = true;

	AIController = Cast<AEnemyController>(OwnerComp.GetAIOwner());

	AIController->MeleeAttack();

	return EBTNodeResult::InProgress;
}

void UMeleeAttack::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	//auto AIController = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if (!AIController->IsStillMeleeAttacking())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

