// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Engine/World.h"
#include "CubbyBase.h"
#include "EnemyAbstract.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GrapplePointBase.h"
#include "MyPlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "Engine/Public/TimerManager.h"
//#include "UObject/ConstructorHelpers.h"



APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : AZenithCharacter(ObjectInitializer)
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create Default position for Cubby
	LeftArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftArrow"));
	LeftArrow->SetupAttachment(RootComponent);

	//set up collider for grapple points visibility
	GrappleVisibleDistanceCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Grapplehook visible Distance"));
	GrappleVisibleDistanceCollider->SetupAttachment(RootComponent);
	GrappleVisibleDistanceCollider->SetSphereRadius(GrappleVisibleDistance);

	GrappleReachableDistanceCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Grapplehook Reachable Distance"));
	GrappleReachableDistanceCollider->SetupAttachment(RootComponent);
	GrappleReachableDistanceCollider->SetSphereRadius(GrappleReachableDistance);

	GrappleMinDistanceCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Grapplehook Min Distance"));
	GrappleMinDistanceCollider->SetupAttachment(RootComponent);
	GrappleMinDistanceCollider->SetSphereRadius(GrappleMinDistance);

	//audio cues
	//static ConstructorHelpers::FObjectFinder<USoundCue>HurtCue(TEXT("SoundCue'/Game/Gameplay/Audio/Character/Su/Hurt/Su_Hurt_Cue.Su_Hurt_Cue'"));

	PrimaryActorTick.bCanEverTick = true;
	_Gravity = 3920;
	_CharacterHookHighestHeight = 150;
	_IsEngage = false;
	ClosestToAim = nullptr;
	TeamId = FGenericTeamId(1);

	GrappleObjectsList = TArray<AActor*>();
	EnemiesInRange = TArray<AEnemyAbstract*>();
	EnemiesInSight = TArray<AEnemyAbstract*>();
	//PreJump = *new FVector(885, 885, 1252);


}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GrappleObjectsList.Num() != 0) 
	{
		ClosestToAim = GetClosestTargetToGrapple(GrappleObjectsList);

		//if (!ClosestToAim) return;
		//if (!ClosestToAim->IsValidLowLevel()) return;
	
		for (AActor* GP : GrappleObjectsList)
		{
			//cast the target to GrapplePointBase to check if it is a grapplepoint 
			if (!GP) continue;
			if (!GP->IsValidLowLevel()) continue;
			AGrapplePointBase *tmp = Cast<AGrapplePointBase>(GP);
			if (tmp)
			{
				if (tmp == ClosestToAim) {
					tmp->SetIsTargeting();
				}
				else {
					tmp->SetIsNotTargeting();
				}
			}// when the target is a grapple point
			else 
			{
				AEnemyAbstract* tmp = Cast<AEnemyAbstract>(GP);
				if (tmp) 
				{
					if (tmp == ClosestToAim) {
						tmp->SetIsTarget();
					}
					else {
						tmp->SetIsNotTarget();
					}
				}
			}// then the target is enemy
		}
	}

	/*
	if (CurrentCubby && !CurrentCubby->IsGrapplling)
	{
		float DistanceToCubby = (this->GetActorLocation() - CurrentCubby->GetActorLocation()).Size();
		if (DistanceToCubby >= MaxDistanceToCubby)
		{
			if (!CurrentCubby) return;
			if (!CurrentCubby->IsValidLowLevel()) return;
			
			CurrentCubby->ConditionalBeginDestroy();
			CurrentCubby=nullptr;

			SetUpCubby();
		}
	}
	*/
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetUpCubby();

	//ResetPlayerProperties();

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnHit);
}

void APlayerCharacter::ResetPlayerProperties()
{
	GrappleObjectsList.Empty();
	EnemiesInRange.Empty();
	EnemiesInSight.Empty();
}

void APlayerCharacter::AddHealth(float Health)
{
	if (CurrentHealth < TotalHealth)
	{
		CurrentHealth += Health;
		FMath::Clamp(CurrentHealth, 0.0f, 100.0f);
		HealthPercentage = CurrentHealth / TotalHealth;
	}
}

void APlayerCharacter::SetUpCubby()
{
	CurrentCubby = GetWorld()->SpawnActor<ACubbyBase>(Cubby, LeftArrow->GetComponentLocation(), GetActorRotation());
	UE_LOG(LogTemp, Warning, TEXT("LeftArrowLocation:  %s"), *LeftArrow->GetComponentLocation().ToString())
	CurrentCubby->SetUpPosition(this);
}

void APlayerCharacter::UsingGrappleHook(AActor* TargetPoint)
{
	if (!TargetPoint) return;
	if (!TargetPoint->IsValidLowLevel()) return;

	IsUsingGrappleHook = true;

	GetCharacterMovement()->AirControl = 0;

	TargetLocation = TargetPoint->GetActorLocation();
	TargetDirction = TargetLocation - this->GetActorLocation();

	auto EnemyTmp = Cast<AEnemyAbstract>(TargetPoint);

	if (EnemyTmp)
	{
		TargetDirction -= TargetDirction.GetSafeNormal() * GrappleHookDistanceMult;
	}

	float VelocityZ = 0;
	float VelocityX = 0;
	float VelocityY = 0;

	//UE_LOG(LogTemp, Warning, TEXT("TargetDirction.z: %f"), TargetDirction.Z)
	if (TargetDirction.Z >= GrappleStatusChangeHeight
		)
	{
		float TimeInterval;
		GetCharacterMovement()->GravityScale = 1;
		VelocityZ = (float)sqrt(_Gravity * 2 * (_CharacterHookHighestHeight + TargetDirction.Z));
		TimeInterval = VelocityZ / _Gravity + sqrt(2 * (_CharacterHookHighestHeight - 50) / _Gravity);
		VelocityX = TargetDirction.X / TimeInterval;
		VelocityY = TargetDirction.Y / TimeInterval;
		SetUpVelocity(VelocityX, VelocityY, VelocityZ);
		LaunchCharacter(Velocity, true, true);
	}
	else
	{
		GetCharacterMovement()->GravityScale = 0;
		FTimerHandle UnusedHandle;
		FVector PreJump = *new FVector(TargetDirction.GetSafeNormal().X * PreJumpXYScale, TargetDirction.GetSafeNormal().Y * PreJumpXYScale, PreJumpHeight);
		LaunchCharacter(PreJump, true, true);
		//UE_LOG(LogTemp, Warning, TEXT("PreJump!!!!!"))
		GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &APlayerCharacter::OnPauseInAir, PreJumpTime, false);
	}

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Rader", IE_Pressed, this, &APlayerCharacter::Rader);
}

void APlayerCharacter::OnHurt(float dmg, float KnockBackRatio)
{
	Super::OnHurt(dmg, KnockBackRatio);

	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	PlayerHurt.Broadcast();

	if (CurrentHealth <= 0)
	{
		//this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
		DisableInput(Cast<APlayerController>(this->GetController()));
		FTimerHandle UnusedHandle;
		GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &APlayerCharacter::Respawn, TimeBeforeRespawn, false);
		FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		if (DeathAudio)
		{
			AudioComponent->SetSound(DeathAudio);
			AudioComponent->Play();
		}
		if (AnimInstance && DeathAnim)
		{
			//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
			AnimInstance->Montage_Play(DeathAnim, 1.0f);
		}
		IsAlive = false;
	}
}

void APlayerCharacter::SetUpVelocity(float VelocityX, float VelocityY, float VelocityZ)
{
	Velocity = *new FVector(VelocityX, VelocityY, VelocityZ);
}

void APlayerCharacter::ToggleIsEngage() 
{
	_IsEngage = !_IsEngage;
}

void APlayerCharacter::SetIsUsingGrappleHook()
{
	IsUsingGrappleHook = true;
}

void APlayerCharacter::UnSetIsUsingGrappleHook()
{
	IsUsingGrappleHook = false;
}

void APlayerCharacter::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (IsUsingGrappleHook)
	{
		GetCharacterMovement()->AirControl = 0.5;
		GetCharacterMovement()->GravityScale = 1;
		GetCharacterMovement()->Velocity = *new FVector(0, 0, 0);
		auto EnemyTmp = Cast<AEnemyAbstract>(OtherActor);
		if (EnemyTmp)
		{
			// if the enmey that player grapple to is marked by rader
			if (EnemyTmp->RaderStatusEnum == RaderStatus::RaderMarked)
			{
				EnemyTmp->OnHurt(RaderImpactDmg, 2);
				EnemyTmp->SetIsNotMarked();
			}
			else
			{
				EnemyTmp->OnHurt(NormalImpactDmg, 1);
			}
		}
		IsUsingGrappleHook = false;
		IsStunned = false;
		PlayerLand.Broadcast();
	}
}

ACubbyBase * APlayerCharacter::GetCurrentCubby()
{
	return CurrentCubby;
}

void APlayerCharacter::AddGrapplePointToArray(AActor* GrapplePoint)
{
	if (!GrapplePoint) return;
	if (!GrapplePoint->IsValidLowLevel()) return;

	//FString Temp = GrapplePoint->GetName();
	//UE_LOG(LogTemp, Warning, TEXT("Adding called and Grapplepoint is:  %s"), *Temp)
	if (!GrappleObjectsList.Contains(GrapplePoint))
	{
		GrappleObjectsList.AddUnique(GrapplePoint);
	}
	//UE_LOG(LogTemp, Warning, TEXT("GrapplePointsReachable.length =  %i"), GrapplePointsReachable->Num())
}

void APlayerCharacter::RemoveGrapplePointFromArray(AActor* GrapplePoint)
{
	if (!GrapplePoint) return;
	if (!GrapplePoint->IsValidLowLevel()) return;
	//FString Temp = GrapplePoint->GetName();
	//UE_LOG(LogTemp, Warning, TEXT("Removing called and Grapplepoint is:  %s"), *Temp)
	//UE_LOG(LogTemp, Warning, TEXT("Removing called"))
	if(GrappleObjectsList.Contains(GrapplePoint))
	{
		GrappleObjectsList.Remove(GrapplePoint);
	}
}

void APlayerCharacter::AddEnemyToTheList(AEnemyAbstract * Enemy)
{
	if (!Enemy) return;
	if (!Enemy->IsValidLowLevel()) return;

	if (Enemy && !EnemiesInRange.Contains(Enemy))
	{
		EnemiesInRange.AddUnique(Enemy);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Enemy add to list called"))
}

void APlayerCharacter::RemoveEnemyFromTheList(AEnemyAbstract * Enemy)
{
	if (!Enemy) return;
	if (!Enemy->IsValidLowLevel()) return;

	if (EnemiesInRange.Contains(Enemy))
	{
		EnemiesInRange.Remove(Enemy);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Enemy Remove from list called"))
}

void APlayerCharacter::AddEnemyToTheInSight(AEnemyAbstract * Enemy)
{
	if (!Enemy) return;
	if (!Enemy->IsValidLowLevel()) return;

	if (Enemy && !EnemiesInSight.Contains(Enemy))
	{
		EnemiesInSight.AddUnique(Enemy);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Enemy add to list called"))
}

void APlayerCharacter::RemoveEnemyFromTheInSight(AEnemyAbstract * Enemy)
{
	if (!Enemy) return;
	if (!Enemy->IsValidLowLevel()) return;

	if (EnemiesInSight.Contains(Enemy))
	{
		EnemiesInSight.Remove(Enemy);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Enemy Remove from list called"))
}

void APlayerCharacter::OnTimerEnd()
{
	//LaunchCharacter(Velocity, true, true);
	TargetDirction = TargetLocation - this->GetActorLocation() + FVector::UpVector * 10;
	Velocity = TargetDirction * 4.5;
	LaunchCharacter(Velocity, true, true);
	UE_LOG(LogTemp, Warning, TEXT("TargetDirection: %s"), *TargetDirction.ToString())
}

void APlayerCharacter::OnPauseInAir()
{
	GetCharacterMovement()->Velocity = *new FVector(0, 0, 0);
	FTimerHandle UnusedHandle;
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &APlayerCharacter::OnTimerEnd, GrappleHookPauseTime, false);
	//FVector TargetDirction = TargetLocation - this->GetActorLocation();
}

void APlayerCharacter::Respawn()
{

	this->Destroy();
	//this->ConditionalBeginDestroy();
	EnableInput(Cast<APlayerController>(this->GetController()));
}

void APlayerCharacter::SetTheTargetingEnemy()
{
	ClosestToLock = GetClosestTargetToAim();
}

AEnemyAbstract* APlayerCharacter::GetClosestTargetToAim()
{

	AEnemyAbstract* Tmp = nullptr;
	ClosestToAim = nullptr;
	float DotResult = 0;
	float MaxDotResult = 0;

	for (AEnemyAbstract* GP : EnemiesInSight)
	{
		if (!GP) continue;
		if (!GP->IsValidLowLevel()) continue;

		AEnemyAbstract *tmpEnemy = Cast<AEnemyAbstract>(GP);
		if (!tmpEnemy) continue;
		if (!tmpEnemy->IsValidLowLevel()) continue;

		if (tmpEnemy->UIStatusEnum == UIVisibility::UnVisible)
		{
			continue;
		}

		FVector PlayerLocation = FollowCamera->GetComponentLocation();
		FVector GrapplePointLocation = GP->GetActorLocation();

		FVector Direction = (GrapplePointLocation - PlayerLocation).GetSafeNormal();
		FVector PlayerLookingDirection = FollowCamera->GetForwardVector().GetSafeNormal();

		DotResult = FVector::DotProduct(PlayerLookingDirection, Direction);

		if (DotResult >= GrapplePointDotMin && DotResult > MaxDotResult)
		{
			MaxDotResult = DotResult;
			Tmp = GP;
		}
	}
	return Tmp;
}


AActor* APlayerCharacter::GetClosestTargetToGrapple(TArray<AActor*> &List)
{

	AActor* Tmp = nullptr;
	ClosestToAim = nullptr;
	float DotResult = 0;
	float MaxDotResult = 0;

	for (AActor* GP : List)
	{
		if (!GP) continue;
		if (!GP->IsValidLowLevel()) continue;

		AEnemyAbstract *tmpEnemy = Cast<AEnemyAbstract>(GP);

		if (tmpEnemy)
		{
			if (tmpEnemy->UIStatusEnum == UIVisibility::UnVisible)
			{
				continue;
			}
		}

		AGrapplePointBase *TmpGrapple = Cast<AGrapplePointBase>(GP);

		if (TmpGrapple)
		{
			if (TmpGrapple->UIStatusEnum == UIVisibility::UnVisible || TmpGrapple->GetIsPlayerOnTop())
			{
				continue;
			}
		}
		
		FVector PlayerLocation = FollowCamera->GetComponentLocation();
		FVector GrapplePointLocation = GP->GetActorLocation();

		FVector Direction = (GrapplePointLocation - PlayerLocation).GetSafeNormal();
		FVector PlayerLookingDirection = FollowCamera->GetForwardVector().GetSafeNormal();

		DotResult = FVector::DotProduct(PlayerLookingDirection, Direction);

		if (DotResult >= GrapplePointDotMin && DotResult > MaxDotResult)
		{
			MaxDotResult = DotResult;
			Tmp = GP;
		}
	}
	return Tmp;
}

AEnemyAbstract* APlayerCharacter::GetRandomEnemyInReachableRange()
{
	float RandomNum = 0;
	if (EnemiesInSight.Num() != 1)
	{
		for (int i = 0; i < 10; i++)
		{
			RandomNum = FMath::RandRange(0, EnemiesInSight.Num() - 1);
			if (!EnemyOnMark) break;
			if (EnemyOnMark && !EnemiesInRange.Contains(EnemiesInSight[RandomNum])) break;
		}

	}
	
	return EnemiesInSight[RandomNum];
}

void APlayerCharacter::Rader()
{
	if (EnemyOnMark && EnemyOnMark->GetRaderStatus() == RaderStatus::RaderMarked)
	{

	}
	if (EnemiesInSight.Num() != 0)
	{
		EnemyOnMark = GetRandomEnemyInReachableRange();
		EnemyOnMark->SetIsMarked();
		//UE_LOG(LogTemp, Warning, TEXT("Rader active"))

		FTimerHandle UnusedHandle;
		GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &APlayerCharacter::ResetEnemyRaderMark, EnemyMarkResetTime, false);
	}
}

void APlayerCharacter::ResetEnemyRaderMark()
{
	if (!EnemyOnMark) return;
	if (!EnemyOnMark->IsValidLowLevel()) return;

	//UE_LOG(LogTemp, Warning, TEXT("Reset enemy rader mark called"))
	EnemyOnMark->SetIsNotMarked();
}

void APlayerCharacter::ResetPlayer()
{
	GrappleObjectsList.Empty();
	EnemiesInRange.Empty();
	EnemiesInSight.Empty();
	ClosestToAim = nullptr;
	ClosestToLock = nullptr;
}