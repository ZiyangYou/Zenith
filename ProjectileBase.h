// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLandingLocationSetUp);

UCLASS()
class ZENITH_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
		FOnLandingLocationSetUp OnLandingLocationSetUp;

	// Sets default values for this actor's properties
	AProjectileBase(const FObjectInitializer & ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Status")
	float ColliderRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Status")
		bool CanHitPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Status")
	float LifeSpan = 3.0f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USphereComponent *SphereCollisionComponent;

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent *ProjectileMesh;
	*/

	// Projectile movement component.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION()
	void OnHit(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float KnockBackRatio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector InitialSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void FiringToDirection(const FVector & ShootDirection);

	float DotProduction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector LandingLocation;

	void SetUpLandingLocation(const FVector & LandingLocation);

};
