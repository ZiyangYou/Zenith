// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyController.h"
#include "LionEnemyController.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class BossStates : uint8
{
	ShootingFireBall UMETA(DisplayName = "ShootingFireBall"),
	ShootingLaser UMETA(DisplayName = "ShootingLaser")
	//Investigating UMETA(DisplayName = "Investigating")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShootingFireBalls, FVector, TargetLocation);

UCLASS()
class ZENITH_API ALionEnemyController : public AEnemyController
{
	GENERATED_BODY()
public:

	ALionEnemyController(const FObjectInitializer & ObjectInitializer);

	UFUNCTION(BlueprintCallable)
		void ShootingFireBall();

	UPROPERTY(BlueprintAssignable)
		FOnShootingFireBalls OnShootingFireBall;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ShootingLasers();
	virtual void ShootingLasers_Implementation();

	UFUNCTION(BlueprintCallable)
	void FillArrayByNumber(TArray<FVector> &Array, int NumOfElements);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
		int CurrentNumOfFireBallWaves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
		int CurrentNumOfLaserWaves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
		int NumOfFireBallWaves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
		int NumOfLaserWaves;

protected:
	
	virtual void BeginPlay() override;

	virtual void OnPawnDected(AActor* DetectedPawn, FAIStimulus Stimulus) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
		FName Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
		int NumOfFireBallsCanShoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
		int NumOfLasersCanShoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
		float FireBallSpawningRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
		float FireBallShootingHeight;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> TargetLocations;

};
