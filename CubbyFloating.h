// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CubbyFloating.generated.h"

/**
 * 
 */
UCLASS()
class ZENITH_API UCubbyFloating : public UBTTaskNode
{
	GENERATED_BODY()
		
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class ACubbyController *AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent *FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector LeftVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class ACubbyBase *Cubby;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class APlayerCharacter *Player;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		float DistanceToPlayer;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		float SpeedMult;

};
