// Fill out your copyright notice in the Description page of Project Settings.

#include "CubbyFloating.h"
#include "PlayerCharacter.h"
#include "CubbyController.h"
#include "CubbyBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCubbyFloating::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	LeftVector = (-(FollowCamera->GetRightVector()) * DistanceToPlayer) + FollowCamera->GetUpVector() * DistanceToPlayer * 1;
	FVector Force;
	Force = (Player->GetActorLocation() + LeftVector - Cubby->GetActorLocation()) * SpeedMult;
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->Velocity = Force;
		CharacterMovementComponent->UpdateComponentVelocity();
	}
	Cubby->SetActorRotation(FollowCamera->GetComponentRotation());
	float DistanceToSu = (Cubby->GetActorLocation() - Player->GetActorLocation()).Size();
	if (DistanceToSu >= 180)
	{
		Cubby->SetActorLocation(Player->LeftArrow->GetComponentLocation());
	}
	else if (DistanceToSu >= 150)
	{
		Cubby->CapsuleCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Cubby->CapsuleCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
		//AIController->SetToComingBackMode();
	}
	else
	{
		Cubby->CapsuleCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Cubby->CapsuleCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	}
}

EBTNodeResult::Type UCubbyFloating::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	bNotifyTick = true;

	AIController = Cast<ACubbyController>(OwnerComp.GetOwner());
	
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	else 
	{
		Cubby = AIController->ControlledPawn;
		Player = Cubby->PlayerCharacter;
		FollowCamera = Player->GetFollowCamera();
		CharacterMovementComponent = Cubby->CharacterMovementComponent;
	}

	Cubby->CapsuleCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Cubby->CapsuleCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	return EBTNodeResult::InProgress;
}
