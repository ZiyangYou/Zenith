// Fill out your copyright notice in the Description page of Project Settings.

#include "CubbyGrappleToTarget.h"
#include "CubbyController.h"
#include "CubbyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCubbyGrappleToTarget::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	FVector Force = (Target->GetActorLocation() - AIController->ControlledPawn->GetActorLocation()) * SpeedMult;
	CharacterMovementComponent->Velocity = Force;
	CharacterMovementComponent->UpdateComponentVelocity();

}

EBTNodeResult::Type UCubbyGrappleToTarget::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	bNotifyTick = true;

	AIController = Cast<ACubbyController>(OwnerComp.GetOwner());

	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		Target = Cast<AActor>(AIController->BlackboardComp->GetValueAsObject(TEXT("Target")));

		//TargetLocation = AIController->BlackboardComp->GetValueAsVector(TEXT("GrappleTargetLocation"));
		if (!Target)
		{
			return EBTNodeResult::Failed;
		}
	}

	Cubby = AIController->ControlledPawn;
	CharacterMovementComponent = Cubby->CharacterMovementComponent;
	Cubby->CapsuleCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	return EBTNodeResult::InProgress;
}
