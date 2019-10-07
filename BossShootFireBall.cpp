// Fill out your copyright notice in the Description page of Project Settings.

#include "BossShootFireBall.h"
#include "LionEnemyController.h"

EBTNodeResult::Type UBossShootFireBall::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{

	AIController = Cast<ALionEnemyController>(OwnerComp.GetAIOwner());

	AIController->ShootingFireBall();

	return EBTNodeResult::Succeeded;

}
