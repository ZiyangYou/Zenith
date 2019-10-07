// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GetPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class ZENITH_API UGetPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
	virtual	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
		struct FBlackboardKeySelector IndexPatrolPoint;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		struct FBlackboardKeySelector PatrolPoint;
	
};
