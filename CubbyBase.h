// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CubbyBase.generated.h"

UCLASS()
class ZENITH_API ACubbyBase : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubbyBase();

	UPROPERTY(Category = "Movement", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cubby Movement")
	bool IsGrapplling;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class APlayerCharacter *PlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleCollider;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class ACubbyController* CubbyController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cubby Movement")
		bool IsComingBack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundCue* GrappleShootAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundCue* CubbyOnHitAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundCue* GrapplePullAudio;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cubby Grapple")
	AActor *TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cubby Movement")
	float SpeedMult;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = cable, meta = (AllowPrivateAccess = "true"))
	class UCableComponent* Cable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cubby Movement")
	float DistanceToPlayer;



	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex);

	void ReleaseEnemy(class AEnemyAbstract *Enemy);

	void FreezeCubby();

	UFUNCTION(BlueprintCallable)
	void ResetCubbyPosition();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DurationRatio;

	UPROPERTY()
		float FrezzeDuration;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
	float ShootCubby(AActor* Target);


	void SetUpPosition(APlayerCharacter *PlayerCharacter);

	UFUNCTION(BlueprintCallable)
	class UCharacterMovementComponent* GetProjectileMovement();
};
