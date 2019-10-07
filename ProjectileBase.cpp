// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileBase.h"
#include "PlayerCharacter.h"
#include "EnemyAbstract.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectileBase::AProjectileBase(const FObjectInitializer& ObjectInitializer) : AActor(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision Component"));
	//ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = SphereCollisionComponent;
	/*
	ProjectileMesh->SetupAttachment(SphereCollisionComponent);
	ProjectileMesh->bHiddenInGame = false;
	//UE_LOG(LogTemp, Warning, TEXT("Projectile mesh relative location: %s"), *ProjectileMesh->RelativeLocation.ToString())
	*/

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	// set up lifespan
	InitialLifeSpan = 3.0f;
	CanHitPlayer = true;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	SphereCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnHit);
	//ProjectileMesh->SetRelativeLocation(FVector::ZeroVector);
	
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::FiringToDirection(const FVector& ShootDirection)
{
	//UE_LOG(LogTemp, Warning, TEXT("ShootDirection is: %s"), *(ShootDirection.ToString()))
	InitialSpeed = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	if (this)
	{
		ProjectileMovementComponent->Velocity = InitialSpeed;
	}
}

void AProjectileBase::SetUpLandingLocation(const FVector & LandingLocation)
{
	this->LandingLocation = LandingLocation;
	OnLandingLocationSetUp.Broadcast();
}

void AProjectileBase::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanHitPlayer)
	{
		AEnemyAbstract *TmpEnemy = Cast<AEnemyAbstract>(OtherActor);
		if (!TmpEnemy)
		{
			APlayerCharacter *TmpPlayer = Cast<APlayerCharacter>(OtherActor);
			if (TmpPlayer)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Target Hit!!"))
				if (TmpPlayer->GetController())
				{
					TmpPlayer->OnHurt(Damage, KnockBackRatio);
				}
			}
			this->Destroy();
		}
	}
}


