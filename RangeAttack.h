// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "RangeAttack.generated.h"

/**
 * 
 */
UCLASS()
class ZENITH_API URangeAttack : public UBTTaskNode
{
	GENERATED_BODY()

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AEnemyController *AIController;
};
