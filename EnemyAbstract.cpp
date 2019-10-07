// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAbstract.h"
#include "EnemyController.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "WidgetComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "BrainComponent.h"
#include "GrapplePointBase.h"
#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "PatrolRoute.h"



AEnemyAbstract::AEnemyAbstract(const FObjectInitializer& ObjectInitializer):AZenithCharacter(ObjectInitializer)
{
	PatrolRoute = CreateDefaultSubobject<UPatrolRoute>(TEXT("PatrolRoute"));
	this->AddOwnedComponent(PatrolRoute);

	FiringLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Component"));
	GrappleUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("Grapple Point UI"));
	HealthBarUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar UI"));
	
	FiringLocation->SetupAttachment(RootComponent);
	GrappleUI->SetupAttachment(RootComponent);
	HealthBarUI->SetupAttachment(RootComponent);

	IsBlocked = false;

	GrappleStatusEnum = GrapplePointStatus::OutRange;
	UIStatusEnum = UIVisibility::Visible;
	HealthBarUI->SetVisibility(false);
	HasGrapplePoint = true;

}

void AEnemyAbstract::BeginPlay()
{
	Super::BeginPlay();

	if (GrappleHookUI) {
		GrappleUI->SetWidgetClass(GrappleHookUI);
	}
	AIController = Cast<AEnemyController>(this->GetController());
	//GrappleHookUI = GrappleUI->GetUserWidgetObject();
	//GrappleHookUI->SetVisibility(ESlateVisibility::Hidden);

}

void AEnemyAbstract::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//check if enemy is in player sight
	if (GrappleStatusEnum != GrapplePointStatus::OutRange)
	{
		UICollisionCheck();
	}

	//Set Enemy is not reachable if UI is not visible
	/*
	if (UIStatusEnum == UIVisibility::UnVisible)
	{
		HealthBarUI->SetVisibility(false);
		//IsBlocked = true;
	}
	else if(UIStatusEnum == UIVisibility::Visible && GrappleStatusEnum != GrapplePointStatus::OutRange)
	{
		HealthBarUI->SetVisibility(true);
		//IsBlocked = false;
	}
	*/

}

void AEnemyAbstract::SetIsVisible()
{
	HealthBarUI->SetVisibility(true);
	GrappleStatusEnum = GrapplePointStatus::Visible;
	this->SetUINotBlocked();
}

void AEnemyAbstract::SetIsMarked()
{
	RaderStatusEnum = RaderStatus::RaderMarked;
}

void AEnemyAbstract::SetIsNotMarked()
{
	RaderStatusEnum = RaderStatus::NotRaderMarked;
}

void AEnemyAbstract::SetIsNotVisible()
{
	HealthBarUI->SetVisibility(false);
	GrappleStatusEnum = GrapplePointStatus::OutRange;
	//this->SetUIBlocked();
	HealthBarUI->SetVisibility(false);
}

void AEnemyAbstract::SetIsTarget()
{
	GrappleStatusEnum = GrapplePointStatus::Targeting;
}

void AEnemyAbstract::SetIsNotTarget()
{
	GrappleStatusEnum = GrapplePointStatus::Reachable;
}

void AEnemyAbstract::SetIsReachable()
{
	GrappleStatusEnum = GrapplePointStatus::Reachable;
	//HealthBarUI->SetVisibility(true);
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());

	if (!PlayerCharacter) return;
	if (!PlayerCharacter->IsValidLowLevel()) return;

	if (this->CurrentHealth > 0 && HasGrapplePoint)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Player character found"))
		PlayerCharacter->AddGrapplePointToArray(this);
		PlayerCharacter->AddEnemyToTheList(this);
	}
}

void AEnemyAbstract::SetIsNotReachable()
{
		GrappleStatusEnum = GrapplePointStatus::Visible;
		//HealthBarUI->SetVisibility(false);
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());

		if (!PlayerCharacter) return;
		if (!PlayerCharacter->IsValidLowLevel()) return;

	if (HasGrapplePoint)
	{
		PlayerCharacter->RemoveGrapplePointFromArray(this);
		PlayerCharacter->RemoveEnemyFromTheList(this);
	}
}

void AEnemyAbstract::SetEnemyIsInAttackRange()
{
		GrappleStatusEnum = GrapplePointStatus::Visible;
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());

		if (!PlayerCharacter) return;
		if (!PlayerCharacter->IsValidLowLevel()) return;

	if (HasGrapplePoint)
	{
		PlayerCharacter->RemoveGrapplePointFromArray(this);
	}
}

void AEnemyAbstract::SetEnemyIsNotInAttackRange()
{
	GrappleStatusEnum = GrapplePointStatus::Reachable;
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());

	if (!PlayerCharacter) return;
	if (!PlayerCharacter->IsValidLowLevel()) return;

	if (this->CurrentHealth > 0 && HasGrapplePoint)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Player character found"))
		PlayerCharacter->AddGrapplePointToArray(this);
		//PlayerCharacter->AddEnemyToTheList(this);
	}
}

void AEnemyAbstract::SetUIBlocked()
{
	//HealthBarUI->SetVisibility(false);
	UIStatusEnum = UIVisibility::UnVisible;
	//UE_LOG(LogExec, Warning, TEXT("SetUIBlocked() was called."));
}

void AEnemyAbstract::SetUINotBlocked()
{
	//HealthBarUI->SetVisibility(true);
	UIStatusEnum = UIVisibility::Visible;
	//UE_LOG(LogExec, Warning, TEXT("SetUINotBlocked() was called."));
}

UIVisibility AEnemyAbstract::GetUIStatus()
{
	return UIStatusEnum;
}

GrapplePointStatus AEnemyAbstract::GetGrappleStatus()
{
	return GrappleStatusEnum;
}

RaderStatus AEnemyAbstract::GetRaderStatus()
{
	return RaderStatusEnum;
}

void AEnemyAbstract::UICollisionCheck()
{
	TArray<FHitResult> Hits;
	TArray<AActor*> ActorsToIgnore;
	//AEnemyAbstract *Pawn = Cast<AEnemyAbstract>(OwnerComp.GetOwner());
	FVector EndLocation = GetActorLocation();
	APlayerCharacter *Player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	FVector StartLocation = Player->GetActorLocation();
	FCollisionQueryParams CollisionParams;
	ActorsToIgnore.Empty();
	ActorsToIgnore.Add(this);
	if (Player)
	{
		ActorsToIgnore.AddUnique(Player);
	}
	CollisionParams.AddIgnoredActors(ActorsToIgnore);
	ECollisionChannel TraceChannel = ECollisionChannel::ECC_Camera;

	bool IsTargetHit = GetWorld()->LineTraceMultiByChannel(Hits, StartLocation, EndLocation, TraceChannel, CollisionParams);
	//UE_LOG(LogTemp, Warning, TEXT("Actor hit: %s"), *Hit.Actor->GetName());
	if (IsTargetHit) 
	{
		AGrapplePointBase *tmp = Cast<AGrapplePointBase>(Hits[0].GetActor());
		if (!tmp)
		{
			SetUIBlocked();
		}
		/*
		for (FHitResult Hit : Hits)
		{
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.f, ECC_GameTraceChannel1, 1.f);
			DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Blue, false, 5.f, ECC_GameTraceChannel1, 1.f);
		}
		*/
	}
	else 
	{
		SetUINotBlocked();
		//UIStatusEnum = UIVisibility::Visible;
	}	
}


void AEnemyAbstract::OnHurt(float dmg, float KnockBackRatio)
{
	Super::OnHurt(dmg, KnockBackRatio);

	//UE_LOG(LogTemp, Warning, TEXT("Enemy On Hurt called"))
	
	APlayerCharacter *Player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (CurrentHealth > 0)
	{
		FVector DirectionToPlayer = (Player->GetActorLocation() - this->GetActorLocation()).GetSafeNormal();
		FVector KnockBackDirection = -DirectionToPlayer * KnockBackForce * KnockBackRatio;
		KnockBackDirection.Z += KnockBackHeight;
		LaunchCharacter(KnockBackDirection, true, true);
	}
	else if (CurrentHealth <= 0)
	{

		if (!Player) return;
		if (!Player->IsValidLowLevel()) return;

		//this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
		Player->RemoveGrapplePointFromArray(this);
		Player->RemoveEnemyFromTheList(this);
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

		Cast<AEnemyController>(this->GetController())->OnAIDied();

		IsAlive = false;
		//this->Destroy();
		//this->ConditionalBeginDestroy();
	}

	EnemyHurtByPlayer.Broadcast();
	//AIController->AttackByPlayer();

}