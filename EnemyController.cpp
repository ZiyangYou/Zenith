// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "EnemyAbstract.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "ZenithGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyPlayerController.h"
#include "PlayerCharacter.h"
#include "Components/ArrowComponent.h"
#include "MyPlayerController.h"
#include "ProjectileBase.h"
#include "Navigationsystem.h"
#include "Engine/World.h"
#include "ZenithGameMode.h"
#include "DrawDebugHelpers.h"
#include "Engine/Public/TimerManager.h"
#include "UnrealMathUtility.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


AEnemyController::AEnemyController(const FObjectInitializer& ObjectInitializer) : AAIController(ObjectInitializer)
{

	// Set up AI Behavior Tree
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));

	//Set up AI Perception Component
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight config"));
	
	CrowdFollowingComp = CreateDefaultSubobject<UCrowdFollowingComponent>(TEXT("CrowdFollowingComponent"));

	SightRadius = 2500;
	LostSightRadius = 3000;
	VisionAngleDegrees = 135;

	SetUpSightConfig();
	ControlledPawn = nullptr;

	SetGenericTeamId(FGenericTeamId(2));

	//UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, SightConfig->GetSenseImplementation(), GetPawn());
	

	//set up Blackboard keys
	AIState = "AIStates";
	PatrolPoint = "PatrolPoint";
	IndexPatrolPoint = "IndexPatrolPoint";
	Target = "Target";
	LastTargetLocation = "LastTargetLocation";
	PlayerController = "PlayerController";
	DistanceToTarget = "DistanceToTarget";
	RandLocationInRadius = "RandLocationInRadius";
	IsStunned = "IsStunned";

	IsStunnedByPlayer = false;
}



void AEnemyController::Possess(APawn *InPawn)
{
	Super::Possess(InPawn);
	
	SetUpBT(GetPawn());
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();
	
	FGameplayTag PatrollingTag = FGameplayTag::RequestGameplayTag(TEXT("Patrolling"));
	BehaviorTreeComp->SetDynamicSubtree(PatrollingTag, ControlledPawn->PatrollingBehaviorTree);
	//UE_LOG(LogTemp, Warning, TEXT("PatrollingTag: %s"), *PatrollingTag.ToString());

	FGameplayTag AttackingTag = FGameplayTag::RequestGameplayTag(TEXT("Attacking"));
	BehaviorTreeComp->SetDynamicSubtree(AttackingTag, ControlledPawn->AttackingBehaviorTree);
	//UE_LOG(LogTemp, Warning, TEXT("AttackingTag: %s"), *AttackingTag.ToString());

	FGameplayTag Investigating = FGameplayTag::RequestGameplayTag(TEXT("Investigating"));
	BehaviorTreeComp->SetDynamicSubtree(Investigating, ControlledPawn->InvestigatingBehaviorTree);
	//UE_LOG(LogTemp, Warning, TEXT("InvestigatingTag: %s"), *AttackingTag.ToString());

	APlayerController* Controller = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	BlackboardComp->SetValueAsObject(PlayerController, Controller);

	SetToPatrollMode();

	AnimInstance = ControlledPawn->GetMesh()->GetAnimInstance();

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::OnPawnDected);

	CurrentGameState = Cast<AZenithGameState>(GetWorld()->GetGameState());
	CurrentGameMode = Cast<AZenithGameMode>(GetWorld()->GetAuthGameMode());

	ControlledPawn->EnemyHurtByPlayer.AddDynamic(this, &AEnemyController::AttackByPlayer);
}

void AEnemyController::SetUpSightConfig()
{
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LostSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = VisionAngleDegrees;
	SightConfig->SetMaxAge(MaxAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	//SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	//SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);

	//UE_LOG(LogTemp, Warning, TEXT("SightConfig_SightRaduis: %f"), SightConfig->SightRadius);
}

void AEnemyController::SetUpBT(APawn *InPawn)
{
	ControlledPawn = Cast<AEnemyAbstract>(InPawn);
	
	if (!ControlledPawn)return;
	if (!ControlledPawn->IsValidLowLevel())return;

	if (ControlledPawn && ControlledPawn->BaseBehaviorTree)
	{
		BlackboardComp->InitializeBlackboard(*(ControlledPawn->BaseBehaviorTree->BlackboardAsset));
		BehaviorTreeComp->StartTree(*(ControlledPawn->BaseBehaviorTree));

		//BlackboardComp->SetValueAsVector("LastTargetLocation", FVector::ZeroVector);
		//UE_LOG(LogTemp, Warning, TEXT("Enemy is not null and enemy has behavior tree!!"));
		//FGameplayTag PatrollingTag = UGameplayTagsManager::Get().RequestGameplayTag("Patrolling");

	}
}

void AEnemyController::ShootProjectile()
{
	
	//FVector ProjcetileShootingLocation = ControlledPawn->GetActorLocation();
	FVector ProjcetileShootingLocation = ControlledPawn->FiringLocation->GetComponentLocation();
	FRotator ProjectileRotation = ControlledPawn->GetActorRotation();
	AProjectileBase *ProjectileSpawned = GetWorld()->SpawnActor<AProjectileBase>(Projectile, ProjcetileShootingLocation, ProjectileRotation);
	FVector ShootingDirection;
	if (MyPlayerController) {

		ShootingDirection = (MyPlayerController->GetPawn()->GetActorLocation() - ProjcetileShootingLocation).GetSafeNormal();
	}
	else {
		MyPlayerController = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
		ShootingDirection = (MyPlayerController->GetPawn()->GetActorLocation() - ProjcetileShootingLocation).GetSafeNormal();
	}
	if (ProjectileSpawned)
	{
		ProjectileSpawned->FiringToDirection(ShootingDirection);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Projectile shooting location is: %s"), *ProjcetileShootingLocation.ToString())
	//UE_LOG(LogTemp, Warning, TEXT("Projectile is: %s"), *ProjectileSpawned->GetName())
	//UE_LOG(LogTemp, Warning, TEXT("Projectile location is: %s"), *ProjectileSpawned->GetActorLocation().ToString())
}

void AEnemyController::MeleeAttack()
{
	if (AnimInstance && MeleeAttackAnim)
	{
		AnimInstance->Montage_Play(MeleeAttackAnim, 1.0f);
		IsMeleeAttack = true;
		ControlledPawn->IsInterruptable = false;
		FTimerHandle UnusedHandle;
		GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AEnemyController::ResetMeleeAttackState, ResetMeleeTimer, false);
	}
}

void AEnemyController::RangeAttack()
{
	if (AnimInstance && RangeAttackAnim)
	{
		AnimInstance->Montage_Play(RangeAttackAnim, 1.0f);
		IsRangeAttack = true;
	}
}

void AEnemyController::ResetMeleeAttackState()
{
	IsMeleeAttack = false;
	ControlledPawn->IsInterruptable = true;
}

bool AEnemyController::IsStillMeleeAttacking()
{
	return IsMeleeAttack;
}

bool AEnemyController::IsStillRangeAttacking()
{
	return IsRangeAttack;
}

void AEnemyController::OnPawnDected(AActor* DetectedPawn, FAIStimulus Stimulus)
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
		if (BlackboardComp->GetValueAsFloat(DistanceToTarget) >= LostSightRadius)
		{
			BlackboardComp->SetValueAsObject(Target, NULL);
			BlackboardComp->SetValueAsEnum(AIState, uint8(AIStates::Investigating));
			OnChangeToInvestigateMode.Broadcast();
			UE_LOG(LogTemp, Warning, TEXT("Player not Detected!!"));
		}
	}
}

void AEnemyController::SetRandomlocation(FVector PlayerLocation)
{
	//float OnX = FMath::RandRange(-WalkableRadius, WalkableRadius);
	//float OnY = FMath::RandRange(-WalkableRadius, WalkableRadius);
	//FVector result = ControlledPawn->GetActorLocation() + FVector(OnX, OnY, 0);

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	if (!NavSystem) return;
	if (!NavSystem->IsValidLowLevel())return;
	//UE_LOG(LogTemp, Warning, TEXT("The World is: %s"), *NavSystem->GetName());
	FNavLocation TmpResult;
	bool HasPoint;
	FVector VectorToLocation;
	FVector MaxVectorToLocation;
	FVector FinalResult;
	FVector EnemyLocation = ControlledPawn->GetActorLocation();
	FVector Direction = - (PlayerLocation - EnemyLocation).GetSafeNormal();
	FVector OrginLocation = EnemyLocation + Direction * 200;

	for (int i = 0; i < 5; i++)
	{
		HasPoint = NavSystem->GetRandomReachablePointInRadius(OrginLocation, WalkableRadius, TmpResult);
		VectorToLocation = TmpResult.Location - EnemyLocation;
		if (HasPoint)
		{
			if (VectorToLocation.Size() >= WalkableRadius / 2)
			{
				FinalResult = TmpResult.Location;
				break;
			}
			else
			{
				if (VectorToLocation.Size() > MaxVectorToLocation.Size())
				{
					MaxVectorToLocation = VectorToLocation;
					FinalResult = TmpResult.Location;
				}
			}
		}
	}
	
	if (HasPoint) 
	{
		BlackboardComp->SetValueAsVector(RandLocationInRadius, FinalResult);
		//UE_LOG(LogTemp, Warning, TEXT("The distance to location is: %f"), (result.Location - ControlledPawn->GetActorLocation()).Size());
	}
	else
	{
		BlackboardComp->SetValueAsVector(RandLocationInRadius, EnemyLocation);
	}
}

void AEnemyController::AttackByPlayer()
{
	//FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	BlackboardComp->SetValueAsObject(Target, GetWorld()->GetFirstPlayerController()->GetPawn());
	SetToAttackMode();
}

void AEnemyController::SetToAttackMode()
{
	if (!IsStunnedByPlayer)
	{
		BlackboardComp->SetValueAsEnum(AIState, uint8(AIStates::Attacking));
		UE_LOG(LogTemp, Warning, TEXT("Switch to Attack mode"));
		ControlledPawn->GetCharacterMovement()->MaxWalkSpeed = AttackingMoveSpeed;
		if (CurrentGameState)
		{
			CurrentGameState->ChangeMusic(1);
			CurrentGameState->addOneEnemyInAttackMode();
		}
		OnChangeToAttackMode.Broadcast();
	}
}

void AEnemyController::SetToPatrollMode()
{
	OnChangeToPatrolMode.Broadcast();
	BlackboardComp->SetValueAsEnum(AIState, uint8(AIStates::Patrolling));
	UE_LOG(LogTemp, Warning, TEXT("Switch to patroll mode"));
	ControlledPawn->GetCharacterMovement()->MaxWalkSpeed = PatrollingMoveSpeed;
	
	/*
	if (CurrentGameState)
	{
		CurrentGameState->removeOneEnemyInAttackMode();
	}

	*/
	FTimerHandle UnusedHandle;
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AEnemyController::CheckEnemiesStates, CheckTime, false);

}

void AEnemyController::SetToStunMode()
{
	OnChangeToStunMode.Broadcast();
	BlackboardComp->SetValueAsEnum(AIState, uint8(AIStates::Stunned));
	UE_LOG(LogTemp, Warning, TEXT("Switch to Stun mode"));
	//ControlledPawn->GetCharacterMovement()->MaxWalkSpeed = AttackingMoveSpeed;
	if (CurrentGameState)
	{
		CurrentGameState->ChangeMusic(1);
		CurrentGameState->addOneEnemyInAttackMode();
	}
	IsStunnedByPlayer = true;
}

void AEnemyController::CheckEnemiesStates()
{
	/*
	if (CurrentGameMode && !CurrentGameMode->CheckIsEnemiesInAttackState())
	{
		if (CurrentGameState)
		{
 			CurrentGameState->ChangeMusic(0);
		}
	}
	*/

	/*
	if (CurrentGameState && CurrentGameState->GetTheNumOfEnemiesInAttackMode() <= 0)
	{
		CurrentGameState->ChangeMusic(0);
	}
	*/
}

void AEnemyController::OnAIDied()
{
	if (BehaviorTreeComp)
	{
		BehaviorTreeComp->StopLogic("death");
	}
}

void AEnemyController::SetIsStunned()
{
	BlackboardComp->SetValueAsBool(IsStunned, true);
}

void AEnemyController::SetIsNotStunned()
{
	BlackboardComp->SetValueAsBool(IsStunned, false);
}

uint8 AEnemyController::GetCurrentAIState()
{
	return BlackboardComp->GetValueAsEnum(AIState);
}
