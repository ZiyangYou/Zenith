// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZenithCharacter.h"
#include "GenericTeamAgentInterface.h"
//#include "Delegate.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerHurt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerLand);

UCLASS()
class ZENITH_API APlayerCharacter : public AZenithCharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
public:
	//APlayerCharacter();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GrappleHook")
	class UArrowComponent* LeftArrow;

	APlayerCharacter(const FObjectInitializer & ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnPlayerHurt PlayerHurt;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnPlayerLand PlayerLand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerJumpStatus")
	float GrappleHookDistanceMult;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ResetPlayerProperties();

	UFUNCTION(BlueprintCallable, Category = "Pick Up")
	void AddHealth(float Health);

	UFUNCTION(BlueprintCallable)
	void SetUpCubby();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = "GrappleHook")
	void AddGrapplePointToArray(AActor* GrapplePoint);

	UFUNCTION(BlueprintCallable, Category = "GrappleHook")
	void RemoveGrapplePointFromArray(AActor* GrapplePoint);

	UFUNCTION(BlueprintCallable, Category = "Lock System")
	void AddEnemyToTheList(AEnemyAbstract* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Lock System")
	void RemoveEnemyFromTheList(AEnemyAbstract* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Rader System")
	void AddEnemyToTheInSight(AEnemyAbstract * Enemy);

	UFUNCTION(BlueprintCallable, Category = "Rader System")
	void RemoveEnemyFromTheInSight(AEnemyAbstract * Enemy);

	UFUNCTION(BlueprintCallable)
	virtual void OnHurt(float dmg, float KnockBackRatio) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GrappleHook")
	AActor* ClosestToAim;

	UFUNCTION(BlueprintCallable, Category = "GrappleHook")
	void UsingGrappleHook(AActor* TargetPoint);

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//player character status
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerJumpStatus")
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerJumpStatus")
	float MaxDistanceToCubby;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerJumpStatus")
	float PreJumpXYScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerJumpStatus")
	float PreJumpHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerJumpStatus")
	float PreJumpTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerJumpStatus")
	float GrappleHookPauseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rader System")
		float EnemyMarkResetTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rader System")
		AEnemyAbstract *EnemyOnMark;


	//Grapple Hook Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	float GrapplePointDotMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	float GrappleStatusChangeHeight;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockSystem")
	class AEnemyAbstract* ClosestToLock;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USphereComponent *GrappleVisibleDistanceCollider;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USphereComponent *GrappleReachableDistanceCollider;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USphereComponent *GrappleMinDistanceCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	float GrappleVisibleDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	float GrappleReachableDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GrappleHook")
	float GrappleMinDistance;


	UFUNCTION(BlueprintCallable, Category = "GrappleHook")
	void ToggleIsEngage();

	UFUNCTION(BlueprintCallable, Category = "GrappleHook")
	void SetIsUsingGrappleHook();

	UFUNCTION(BlueprintCallable, Category = "GrappleHook")
	void UnSetIsUsingGrappleHook();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cubby")
	TSubclassOf<class ACubbyBase> Cubby;



	// On land or on hit 
	UFUNCTION(BlueprintCallable, Category = "OnLand")
	void OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);

	UFUNCTION(BlueprintCallable, Category = "OnLand")
	ACubbyBase* GetCurrentCubby();


	// Player Death and respawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerDeath")
	float TimeBeforeRespawn;

	FGenericTeamId TeamId;

	//void OnTimerEnd(FVector Targetlocation);

	void OnTimerEnd();

	//void OnPauseInAir(FVector Targetlocation);

	void OnPauseInAir();

	UPROPERTY()
	TArray<AActor*> GrappleObjectsList;

	UPROPERTY()
	TArray<AEnemyAbstract*> EnemiesInRange;

	UPROPERTY()
	TArray<AEnemyAbstract*> EnemiesInSight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GrappleHook")
	class ACubbyBase* CurrentCubby;

	UFUNCTION(BlueprintCallable, Category = "Lock System")
	void SetTheTargetingEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock System")
	float RaderImpactDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock System")
		float NormalImpactDmg;

	UFUNCTION(BlueprintCallable)
	AEnemyAbstract * GetRandomEnemyInReachableRange();

	UFUNCTION(BlueprintCallable)
	void Rader();

	UFUNCTION(BlueprintCallable)
	void ResetEnemyRaderMark();

	UFUNCTION(BlueprintCallable)
	void ResetPlayer();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GrappleHook")
	bool IsUsingGrappleHook;

private:

	UFUNCTION(Blueprintcallable)
	AEnemyAbstract * GetClosestTargetToAim();

	AActor * GetClosestTargetToGrapple(TArray<AActor*> &List);


	void Respawn();


	void SetUpVelocity(float, float, float);

	UPROPERTY()
	float _Gravity;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	FVector TargetDirction;

	UPROPERTY()
	float _CharacterHookHighestHeight;

	UPROPERTY()
	bool _IsEngage;



};