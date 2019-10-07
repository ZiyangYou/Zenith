// Fill out your copyright notice in the Description page of Project Settings.

#include "BirdEnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

void ABirdEnemyController::BeginPlay()
{
	Super::BeginPlay();
	//GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ABirdEnemyController::OnPawnDectedBird);
	CanSeePlayer = false;
}

void ABirdEnemyController::OnPawnDected(AActor* DetectedPawn, FAIStimulus Stimulus)
{
	if (!DetectedPawn) return;
	if (!DetectedPawn->IsValidLowLevel())return;

	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Detected!!"));
		BlackboardComp->SetValueAsObject(Target, DetectedPawn);
		BlackboardComp->SetValueAsVector(LastTargetLocation, DetectedPawn->GetActorLocation());
		SetToAttackMode();
		CanSeePlayer = true;
	}
	else
	{
		BlackboardComp->SetValueAsObject(Target, NULL);
		BlackboardComp->SetValueAsBool(TEXT("IsAlert"), false);
		//SetToPatrollMode();
		CanSeePlayer = false;
		UE_LOG(LogTemp, Warning, TEXT("Player not Detected!!"));
	}
}
