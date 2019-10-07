// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZenithCharacter.h"
#include "GrapplePointBase.h"
#include "UObject/ConstructorHelpers.h"
#include "PlayerDamageableObjectInterface.h"
#include "EnemyAbstract.generated.h"

UENUM(BlueprintType)
enum class RaderStatus : uint8
{
	NotRaderMarked UMETA(DisplayName = "NotRaderMarked"),
	RaderMarked UMETA(DisplayName = "RaderMarked")
};

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyHurtByPlayer);

UCLASS()
class ZENITH_API AEnemyAbstract : public AZenithCharacter, public IPlayerDamageableObjectInterface
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	AEnemyAbstract(const FObjectInitializer&);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnEnemyHurtByPlayer EnemyHurtByPlayer;

	UPROPERTY(VisibleAnywhere, Category = "Patrol Route")
	class UPatrolRoute* PatrolRoute;

	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
	class UBehaviorTree* BaseBehaviorTree;

	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
	class UBehaviorTree* PatrollingBehaviorTree;

	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
	class UBehaviorTree* AttackingBehaviorTree;

	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
	class UBehaviorTree* InvestigatingBehaviorTree;

	UPROPERTY(VisibleDefaultsOnly)
	class UArrowComponent *FiringLocation;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AEnemyController *AIController;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent *GrappleUI;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent *HealthBarUI;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UUserWidget>GrappleHookUI;
	
	UFUNCTION(BlueprintCallable)
	void SetIsVisible();

	UFUNCTION(BlueprintCallable)
	void SetIsMarked();

	UFUNCTION(BlueprintCallable)
	void SetIsNotMarked();

	UFUNCTION(BlueprintCallable)
	void SetIsNotVisible();

	UFUNCTION(BlueprintCallable)
	void SetIsTarget();

	UFUNCTION(BlueprintCallable)
	void SetIsNotTarget();

	UFUNCTION(BlueprintCallable)
	void SetIsReachable();

	UFUNCTION(BlueprintCallable)
	void SetIsNotReachable();

	UFUNCTION(BlueprintCallable)
	void SetEnemyIsInAttackRange();

	UFUNCTION(BlueprintCallable)
	void SetEnemyIsNotInAttackRange();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetUIBlocked();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetUINotBlocked();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	UIVisibility GetUIStatus();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UIVisibility UIStatusEnum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	RaderStatus RaderStatusEnum;

	UFUNCTION(BlueprintCallable)
	GrapplePointStatus GetGrappleStatus();

	UFUNCTION(BlueprintCallable)
	RaderStatus GetRaderStatus();

	void UICollisionCheck();
	
	UFUNCTION(BlueprintCallable)
	virtual void OnHurt(float dmg, float KnockBackRatio) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsBlocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool HasGrapplePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnockBack")
	float KnockBackForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnockBack")
	float KnockBackHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnockBack")
	int GroupNum;

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	GrapplePointStatus GrappleStatusEnum;
};
