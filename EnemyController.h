// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeToAttackMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeToInvestigateMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeToPatrolMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeToStunMode);


UENUM(BlueprintType)
enum class AIStates : uint8
{
	Patrolling UMETA(DisplayName = "Patrolling"),
	Attacking UMETA(DisplayName = "Attacking"),
	Investigating UMETA(DisplayName = "Investigating"),
	Stunned UMETA(DisplayName = "Stunned")
};
/**
 * 
 */

UCLASS()
class ZENITH_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnChangeToAttackMode OnChangeToAttackMode;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnChangeToPatrolMode OnChangeToPatrolMode;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnChangeToInvestigateMode OnChangeToInvestigateMode;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnChangeToInvestigateMode OnChangeToStunMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI States")
	class UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI States")
	class UBlackboardComponent* BlackboardComp;

	AEnemyController(const FObjectInitializer & ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void ShootProjectile();

	void MeleeAttack();

	void RangeAttack();

	void ResetMeleeAttackState();

	bool IsStillMeleeAttacking();

	bool IsStillRangeAttacking();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI States")
	AIStates AIStateEnum;

	void SetRandomlocation(FVector PlayerLocation);

	UFUNCTION()
	void AttackByPlayer();

	UFUNCTION(BlueprintCallable)
	void SetToAttackMode();

	UFUNCTION(BlueprintCallable)
	void SetToPatrollMode();

	UFUNCTION(BlueprintCallable)
		void SetToStunMode();

	UFUNCTION(BlueprintCallable)
	void CheckEnemiesStates();

	void OnAIDied();

	void SetIsStunned();

	void SetIsNotStunned();

	UFUNCTION(Blueprintcallable)
	uint8 GetCurrentAIState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Animation")
		bool IsMeleeAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Animation")
		bool IsRangeAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Animation")
		bool IsStunnedByPlayer;

protected:

	UPROPERTY()
		class AZenithGameMode *CurrentGameMode;

	UPROPERTY()
		class AZenithGameState *CurrentGameState;

	UPROPERTY()
		class UBlackboardComponent* CurrentBlackboardComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Perception Sight")
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Perception Sight")
		class UCrowdFollowingComponent* CrowdFollowingComp;
	
	UFUNCTION()
	virtual void OnPawnDected(AActor* DetectedPawn, FAIStimulus Stimulus);

	virtual void Possess(APawn* pawn) override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
	float ResetMeleeTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
	float PatrollingMoveSpeed = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
	float AttackingMoveSpeed = 600;

	UPROPERTY(VisibleAnywhere, Category = "AI Perception Sight")
	float MaxAge = 5;
	
	UPROPERTY(VisibleAnywhere, Category = "AI Perception Sight")
	class AEnemyAbstract* ControlledPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception Sight")
		float SightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception Sight")
		float LostSightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception Sight")
		float VisionAngleDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Projectile")
	TSubclassOf<class AProjectileBase> Projectile;
	
	//uint8 EnemyID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Perception Sight")
	class AMyPlayerController *MyPlayerController;

	void SetUpSightConfig();

	void SetUpBT(APawn* InPawn);

	// setup walkable radius between attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Perception Sight")
	float WalkableRadius = 150;

	//Blackboard keys
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName PatrolPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName IndexPatrolPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName LastTargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName DistanceToTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName AIState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName RandLocationInRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName IsStunned;

	//Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Animation")
	class UAnimMontage *MeleeAttackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Animation")
	class UAnimMontage *RangeAttackAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Perception Sight")
	class UAnimInstance *AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		float CheckTime = 4.0f;
};
