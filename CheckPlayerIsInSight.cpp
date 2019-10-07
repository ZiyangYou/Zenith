// Fill out your copyright notice in the Description page of Project Settings.

#include "CheckPlayerIsInSight.h"
#include "AIController.h"
#include "Components/ArrowComponent.h"
#include "EnemyAbstract.h"
#include "MyPlayerController.h"
#include "PlayerCharacter.h"
#include "EnemyController.h"
#include "DrawDebugHelpers.h"
//#include "Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"

void UCheckPlayerIsInSight::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	auto BlackBoardComp = OwnerComp.GetBlackboardComponent();
	auto AIController = OwnerComp.GetAIOwner();
	AEnemyAbstract *ControlledPawn = Cast<AEnemyAbstract>(AIController->GetPawn());

	FHitResult Hit;
	//AEnemyAbstract *Pawn = Cast<AEnemyAbstract>(OwnerComp.GetOwner());
	FVector StartLocation = ControlledPawn->FiringLocation->GetComponentLocation();
	FVector EndLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FCollisionQueryParams CollisionParams;
	ECollisionChannel TraceChannel = ECollisionChannel::ECC_Visibility;

	bool IsTargetHit = GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, TraceChannel, CollisionParams);
	//UE_LOG(LogTemp, Warning, TEXT("Actor hit: %s"), *Hit.Actor->GetName());
	if (IsTargetHit)
	{
		APlayerCharacter* ActorHit = Cast<APlayerCharacter>(Hit.Actor);
		if (!ActorHit)
		{
			BlackBoardComp->SetValueAsObject("Target", NULL);
			BlackBoardComp->SetValueAsBool(TEXT("IsAlert"), false);
			BlackBoardComp->SetValueAsEnum("AIStates", uint8(AIStates::Investigating));
			UE_LOG(LogTemp, Warning, TEXT("Player not Detected!!"));
			//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.f, ECC_WorldStatic, 1.f);
		}
	}
	else
	{
		BlackBoardComp->SetValueAsObject("Target", NULL);
		BlackBoardComp->SetValueAsEnum("AIStates", uint8(AIStates::Investigating));
		UE_LOG(LogTemp, Warning, TEXT("Player not Detected!! No Target hit"));
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.f, ECC_WorldStatic, 1.f);
	}
}
