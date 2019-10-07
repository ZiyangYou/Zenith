// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossShootFireBall.generated.h"

/**
 * 
 */
UCLASS()
class ZENITH_API UBossShootFireBall : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class ALionEnemyController *AIController;

};
