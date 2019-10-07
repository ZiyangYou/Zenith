// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CubbyController.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeToFloatingMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeToGrappleMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeToComeBackMode);

UENUM(BlueprintType)
enum class CubbyState : uint8
{
	FloatingNearPlayer UMETA(DisplayName = "FloatingNearPlayer"),
	Grappling UMETA(DisplayName = "Grappling"),
	ComingBack UMETA(DisplayName = "ComingBack")
};

UCLASS()
class ZENITH_API ACubbyController : public AAIController
{
	GENERATED_BODY()
	
public:

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnChangeToFloatingMode OnChangeToFloatingMode;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnChangeToGrappleMode OnChangeToGrappleMode;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnChangeToComeBackMode OnChangeToComeBackMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI States")
		class UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI States")
		class UBlackboardComponent* BlackboardComp;

	ACubbyController(const FObjectInitializer & ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI States")
		CubbyState CubbyStateEnum;

	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
	class UBehaviorTree* BaseBehaviorTree;

	UFUNCTION(BlueprintCallable)
		void SetToFloatingMode();

	UFUNCTION(BlueprintCallable)
		void SetToGrapplingMode(AActor* Target);

	UFUNCTION(BlueprintCallable)
		void SetToComingBackMode();

	class ACubbyBase* ControlledPawn;

protected:

	virtual void Possess(APawn* pawn) override;

	virtual void BeginPlay() override;

	void SetUpBT(APawn* Pawn);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName CubbyState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName GrappleTargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Blackboard Keys")
	FName Target;
};
