// Fill out your copyright notice in the Description page of Project Settings.

#include "LionEnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "EnemyAbstract.h"
#include "Animation/AnimInstance.h"
#include "Components/ArrowComponent.h"
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "ProjectileBase.h"
#include "Navigationsystem.h"


ALionEnemyController::ALionEnemyController(const FObjectInitializer & ObjectInitializer) : AEnemyController(ObjectInitializer)
{
	TargetLocations = TArray<FVector>();
	SightConfig->SightRadius = 5000;
	SightConfig->LoseSightRadius = 6000;
	SightConfig->PeripheralVisionAngleDegrees = 180;
	GetAIPerceptionComponent()->RequestStimuliListenerUpdate();

}

void ALionEnemyController::BeginPlay()
{
	Super::BeginPlay();


}

void  ALionEnemyController::OnPawnDected(AActor* DetectedPawn, FAIStimulus Stimulus)
{
	if (!DetectedPawn) return;
	if (!DetectedPawn->IsValidLowLevel())return;

	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Detected!!"));
		BlackboardComp->SetValueAsObject(Target, DetectedPawn);
		BlackboardComp->SetValueAsVector(LastTargetLocation, DetectedPawn->GetActorLocation());
		SetToAttackMode();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player not Detected!!"));
	}
}

void ALionEnemyController::ShootingFireBall()
{
	FVector ProjcetileShootingLocation = ControlledPawn->FiringLocation->GetComponentLocation();
	FRotator ProjectileRotation = ControlledPawn->GetActorRotation();
	FVector EnemyLocation = ControlledPawn->GetActorLocation();

	// get X amount number of random locations
	FillArrayByNumber(TargetLocations, NumOfFireBallsCanShoot);

	// shooting fire balls towards that position
	for (FVector tmp : TargetLocations)
	{
		AProjectileBase *ProjectileSpawned = GetWorld()->SpawnActor<AProjectileBase>(Projectile, ProjcetileShootingLocation, ProjectileRotation);
		ProjectileSpawned->SetUpLandingLocation(tmp);
		float VelocityZ = (float)sqrt(3920 * 2 * FireBallShootingHeight);
		float TimeInterval = VelocityZ / 3920 + sqrt(2 * (FireBallShootingHeight) / 3920);
		FVector TargetDirection = tmp - EnemyLocation;
		float VelocityX = TargetDirection.X / TimeInterval;
		float VelocityY = TargetDirection.Y / TimeInterval;
		FVector Velocity = FVector(VelocityX, VelocityY, VelocityZ);
		ProjectileSpawned->FiringToDirection(Velocity);
		OnShootingFireBall.Broadcast(tmp);
	}
	TargetLocations.Empty();
}

void ALionEnemyController::ShootingLasers_Implementation()
{
	FVector EnemyLocation = ControlledPawn->GetActorLocation();

	// get X amount number of random locations
	FillArrayByNumber(TargetLocations, NumOfLasersCanShoot);
}

void ALionEnemyController::FillArrayByNumber(TArray<FVector> &Array, int NumOfElements)
{
	if (!MyPlayerController) 
	{
		MyPlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	}
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	if (!NavSystem) return;
	if (!NavSystem->IsValidLowLevel())return;

	FVector TargetLocation = MyPlayerController->GetPawn()->GetActorLocation();
	FNavLocation TmpResult;
	bool HasPoint;

	for (int i = 0; i < NumOfElements; i++)
	{
		HasPoint = NavSystem->GetRandomPointInNavigableRadius(TargetLocation, FireBallSpawningRange, TmpResult);

		if (HasPoint)
		{
			TargetLocations.Add(TmpResult.Location);
			//DrawDebugBox(GetWorld(), TmpResult.Location, FVector(20, 20, 20), FColor::Blue, true);
		}
	}
	TargetLocations.Add(TargetLocation);
}
