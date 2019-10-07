// Fill out your copyright notice in the Description page of Project Settings.

#include "CubbyBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "EnemyController.h"
#include "PlayerCharacter.h"
#include "EnemyAbstract.h"
#include "CubbyController.h"
#include "Engine/Public/TimerManager.h"
//#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

//Ryan was here
// Sets default values
ACubbyBase::ACubbyBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	
	//CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule component"));
	//RootComponent = CapsuleCollider;
	//CharacterMovementComponent->UpdateComponentVelocity();

	//audio component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ACubbyBase::BeginPlay()
{
	Super::BeginPlay();

	//CharacterMovementComponent = 
	CapsuleCollider = GetCapsuleComponent();
	CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->GravityScale = 0;

	CubbyController = Cast<ACubbyController>(GetController());
	CubbyController->SetToFloatingMode();

	CapsuleCollider->OnComponentHit.AddDynamic(this, &ACubbyBase::OnHit);
	CapsuleCollider->OnComponentBeginOverlap.AddDynamic(this, &ACubbyBase::OnOverlapBegin);
	CapsuleCollider->OnComponentEndOverlap.AddDynamic(this, &ACubbyBase::OnOverlapEnd);
}

// Called every frame
void ACubbyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/*
	if (!PlayerCharacter) return;
	if (!PlayerCharacter->IsValidLowLevel()) return;


	if (IsGrapplling)
	{
		if (!TargetActor) return;
		if (!TargetActor->IsValidLowLevel())return;
		FVector TargetLocation = TargetActor->GetActorLocation();
		FVector Force = (TargetLocation - GetActorLocation()) * SpeedMult;
		ProjectileMovementComponent->Velocity = Force;

	}
	else if (!IsGrapplling)
	{
		UCameraComponent *FollowCamera = PlayerCharacter->GetFollowCamera();
		FVector LeftVector = (-(FollowCamera->GetRightVector()) * DistanceToPlayer) + FollowCamera->GetUpVector() * DistanceToPlayer * 1;
		FVector Force;
		if (IsComingBack)
		{
			Force = (PlayerCharacter->GetActorLocation() + LeftVector - GetActorLocation()) * SpeedMult / 3;
		}
		else 
		{
			Force = (PlayerCharacter->GetActorLocation() + LeftVector - GetActorLocation()) * SpeedMult;
		}
		ProjectileMovementComponent->Velocity = Force;
		//UE_LOG(LogTemp, Warning, TEXT("Cubby Velocity: %s"), *ProjectileMovementComponent->Velocity.ToString())
		this->SetActorRotation(FollowCamera->GetComponentRotation());
		float DistanceToSu = (GetActorLocation() - PlayerCharacter->GetActorLocation()).Size();
		if (DistanceToSu >= 120)
		{
			CapsuleCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			IsComingBack = true;
		}
		else {
			CapsuleCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			IsComingBack = false;
		}
	}
	*/
}

float ACubbyBase::ShootCubby(AActor *Target)
{
	if (!Target) return 0;
	if (!Target->IsValidLowLevel()) return 0;

	IsGrapplling = true;
	TargetActor = Target;

	CapsuleCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	CubbyController->SetToGrapplingMode(Target);

	AudioComponent->SetSound(GrappleShootAudio);
	AudioComponent->Play();

	float Duration = (this->GetActorLocation() - PlayerCharacter->GetActorLocation()).Size() * DurationRatio;

	return Duration;
}

void ACubbyBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("Cubby On Hit called"))
	//FreezeCubby();
}

void ACubbyBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Cubby On Overlap Begin called"))
	if (IsGrapplling)
	{
		FreezeCubby();
		AEnemyAbstract * EnemyTemp = Cast<AEnemyAbstract>(OtherActor);
		if (EnemyTemp)
		{
			AEnemyController *AIController = Cast<AEnemyController>(EnemyTemp->GetController());
			if (AIController)
			{
				AIController->SetIsStunned();

			}
			//FTimerHandle UnusedHandle;
			//GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &ACubbyBase::ReleaseEnemy, FrezzeDuration, false);
		}
	}
}

void ACubbyBase::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
		AEnemyAbstract * EnemyTemp = Cast<AEnemyAbstract>(OtherActor);
		if (EnemyTemp)
		{
			ReleaseEnemy(EnemyTemp);
		}
}

void ACubbyBase::ReleaseEnemy(AEnemyAbstract *Enemy)
{
	if (!Enemy) return;

	AEnemyController *AIController = Cast<AEnemyController>(Enemy->GetController());
	if (AIController)
	{
		AIController->SetIsNotStunned();
	}
}

void ACubbyBase::FreezeCubby() 
{
	if (!PlayerCharacter) return;
	if (!PlayerCharacter->IsValidLowLevel()) return;

		AudioComponent->SetSound(CubbyOnHitAudio);
		AudioComponent->Play();
		//CapsuleCollider->SetSimulatePhysics(false);
		CharacterMovementComponent->StopMovementImmediately();
		FTimerHandle UnusedHandle;
		FrezzeDuration = (this->GetActorLocation() - PlayerCharacter->GetActorLocation()).Size() * DurationRatio;
		//UE_LOG(LogTemp, Warning, TEXT("Duration is: %f"), Duration);
		PlayerCharacter->UsingGrappleHook(TargetActor);

		//GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &ACubbyBase::ResetCubbyPosition, FrezzeDuration, false);
}

void ACubbyBase::ResetCubbyPosition()
{
	AudioComponent->SetSound(GrapplePullAudio);
	AudioComponent->Play();
	IsGrapplling = false;
	//CapsuleCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CubbyController->SetToComingBackMode();

	//IsComingBack = true;
	//PlayerCharacter->SetIsNotStunned();
	//FTimerHandle UnusedHandle;
	//GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &ACubbyBase::ResetCubbyPosition, DurationReturnSu, false);
	//UE_LOG(LogTemp, Warning, TEXT("ResetCubbyPosition called"))
}


void ACubbyBase::SetUpPosition(APlayerCharacter *PlayerCharacter)
{
	if (!PlayerCharacter) return;
	if (!PlayerCharacter->IsValidLowLevel()) return;

	this->PlayerCharacter = PlayerCharacter;
	//CubbyController->SetToFloatingMode();
}

UCharacterMovementComponent * ACubbyBase::GetProjectileMovement()
{
	return CharacterMovementComponent;
}

