// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyController.h"
#include "BirdEnemyController.generated.h"

/**
 * 
 */

UCLASS()
class ZENITH_API ABirdEnemyController : public AEnemyController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual void OnPawnDected(AActor* DetectedPawn, FAIStimulus Stimulus) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanSeePlayer;
};
