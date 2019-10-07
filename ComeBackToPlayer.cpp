// Fill out your copyright notice in the Description page of Project Settings.

#include "ComeBackToPlayer.h"
#include "CubbyController.h"
#include "PlayerCharacter.h"
#include "CubbyBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UComeBackToPlayer::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	LeftVector = (-(FollowCamera->GetRightVector()) * DistanceToPlayer) + FollowCamera->GetUpVector() * DistanceToPlayer * 1;
	FVector MovingDirection = Player->GetActorLocation() + LeftVector - Cubby->GetActorLocation();
	
	if (MovingDirection.Size() <= 100)
	{
		AIController->SetToFloatingMode();
	}
	
	FVector Force = MovingDirection * SpeedMult;

	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->Velocity = Force;
		CharacterMovementComponent->UpdateComponentVelocity();
	}

}

EBTNodeResult::Type UComeBackToPlayer::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	bNotifyTick = true;

	AIController = Cast<ACubbyController>(OwnerComp.GetOwner());

	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	/*
	else
	{
		TargetLocation = AIController->BlackboardComp->GetValueAsVector(TEXT("GrappleTargetLocation"));
		if (TargetLocation == FVector::ZeroVector)
		{
			return EBTNodeResult::Failed;
		}
	}
	*/

	Cubby = AIController->ControlledPawn;
	CharacterMovementComponent = Cubby->CharacterMovementComponent;
	Player = Cubby->PlayerCharacter;
	FollowCamera = Player->GetFollowCamera();

	Cubby->CapsuleCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	return EBTNodeResult::InProgress;
}
