// Fill out your copyright notice in the Description page of Project Settings.

#include "UpdateLPL.h"
#include "EnemyController.h"
#include "AIController.h"
#include "MyPlayerController.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

void UUpdateLPL::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// initialize the blackboard component and Target player object
	auto BlackboardComp = OwnerComp.GetBlackboardComponent();
	auto Target = Cast<AActor> (BlackboardComp->GetValueAsObject("Target"));

	//auto AIController = Cast<AEnemyController> OwnerComp.GetAIOwner();
	//auto PlayerController = Cast<AActor>(BlackboardComp->GetValueAsObject("PlayerController"));

	//check if Target is valid
	if (Target)
	{
		//get player current location
		FVector LastTargetLocation = Target->GetActorLocation();
		FString String = LastTargetLocation.ToString();
		//UE_LOG(LogTemp, Warning, TEXT("Targetlocation is: %s"), *String)
		// if Target is valid return value current location to Blackboard as LastTargetLocation, else return ZeroVector
		BlackboardComp->SetValueAsVector("LastTargetLocation", LastTargetLocation);
	}

}
