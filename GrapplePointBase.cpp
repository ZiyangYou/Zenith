// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplePointBase.h"
#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayerController.h"
#include "Engine/World.h"

// Sets default values
AGrapplePointBase::AGrapplePointBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule component"));
	RootComponent = CapsuleCollider;

	GrappleUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("Grapple Point UI"));
	GrappleUI->SetupAttachment(RootComponent);

	IsPlayerOnTop = false;

	ActorsToIgnore.Add(this);
}

// Called when the game starts or when spawned
void AGrapplePointBase::BeginPlay()
{
	Super::BeginPlay();
	if (GrappleHookUI) {
		GrappleUI->SetWidgetClass(GrappleHookUI);
		GrappleStatusEnum = GrapplePointStatus::OutRange;
	}

}

void AGrapplePointBase::SetIsVisible()
{
	GrappleStatusEnum = GrapplePointStatus::Visible;
}

void AGrapplePointBase::SetIsNotVisible()
{
	GrappleStatusEnum = GrapplePointStatus::OutRange;
}

void AGrapplePointBase::SetIsReachable()
{
	GrappleStatusEnum = GrapplePointStatus::Reachable;
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());

	if (!PlayerCharacter) return;
	if (!PlayerCharacter->IsValidLowLevel()) return;

	//UE_LOG(LogTemp, Warning, TEXT("Player character found"))
	PlayerCharacter->AddGrapplePointToArray(this);
}

void AGrapplePointBase::SetIsNotReachable()
{
	GrappleStatusEnum = GrapplePointStatus::Visible;
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());

	if (!PlayerCharacter) return;
	if (!PlayerCharacter->IsValidLowLevel()) return;

	PlayerCharacter->RemoveGrapplePointFromArray(this);
}

void AGrapplePointBase::SetUIBlocked()
{
	UIStatusEnum = UIVisibility::UnVisible;
	//UE_LOG(LogExec, Warning, TEXT("SetUIBlocked() was called."));
}

void AGrapplePointBase::SetUINotBlocked()
{
	UIStatusEnum = UIVisibility::Visible;
	//UE_LOG(LogExec, Warning, TEXT("SetUINotBlocked() was called."));
}

void AGrapplePointBase::SetPlayerOnTop()
{
	IsPlayerOnTop = true;
}

void AGrapplePointBase::SetPlayerNotOnTop()
{
	IsPlayerOnTop = false;
}

bool AGrapplePointBase::GetIsPlayerOnTop()
{
	return IsPlayerOnTop;
}

UIVisibility AGrapplePointBase::GetUIStatus()
{
	return UIStatusEnum;
}

GrapplePointStatus AGrapplePointBase::GetGrappleStatus()
{
	return GrappleStatusEnum;
}

void AGrapplePointBase::SetIsTargeting()
{
	GrappleStatusEnum = GrapplePointStatus::Targeting;

	//UE_LOG(LogTemp, Warning, TEXT("Is Targeting called"))
}

void AGrapplePointBase::SetIsNotTargeting()
{
	GrappleStatusEnum = GrapplePointStatus::Reachable;
}


// Called every frame
void AGrapplePointBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GrappleStatusEnum != GrapplePointStatus::OutRange)
	{
		UICollisionCheck();
	}
}

void AGrapplePointBase::UICollisionCheck()
{
	FHitResult Hit;
	TArray<FHitResult> Hits;
	//AEnemyAbstract *Pawn = Cast<AEnemyAbstract>(OwnerComp.GetOwner());
	FVector EndLocation = GetActorLocation();
	APawn *Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	FVector StartLocation = Player->GetActorLocation();
	FVector Direction = EndLocation - StartLocation;
	Direction.Z = 0;
	float DistanceToTargetXY = Direction.Size();
	Direction = Direction.GetSafeNormal();

	FVector FirstCheckPoint = StartLocation + Direction * DistanceToTargetXY / 3;
	FVector SecondCheckPoint = FirstCheckPoint + FVector::UpVector * (EndLocation.Z - StartLocation.Z);

	FCollisionQueryParams CollisionParams;
	ActorsToIgnore.Empty();
	ActorsToIgnore.Add(this);
	if (Player)
	{
		ActorsToIgnore.AddUnique(Player);
	}
	CollisionParams.AddIgnoredActors(ActorsToIgnore);

	ECollisionChannel TraceChannel = ECollisionChannel::ECC_Visibility;

	bool IsTargetHit = GetWorld()->SweepSingleByChannel(Hit, StartLocation, FirstCheckPoint, FQuat::Identity, TraceChannel, FCollisionShape::MakeCapsule(25, 20), CollisionParams);
	//UE_LOG(LogTemp, Warning, TEXT("Actor hit: %s"), *Hit.Actor->GetName());
	if (IsTargetHit)
	{
			SetUIBlocked();
	}
	else
	{
		IsTargetHit = GetWorld()->SweepSingleByChannel(Hit, FirstCheckPoint, SecondCheckPoint, FQuat::Identity, TraceChannel, FCollisionShape::MakeCapsule(20, 20), CollisionParams);
		if (IsTargetHit)
		{
			
				SetUIBlocked();
			
		}
		else {
			IsTargetHit = GetWorld()->SweepSingleByChannel(Hit, SecondCheckPoint, EndLocation, FQuat::Identity, TraceChannel, FCollisionShape::MakeCapsule(20, 20), CollisionParams);
			if (IsTargetHit)
			{
				
					SetUIBlocked();
				
			}
			else
			{
				SetUINotBlocked();
			}
		}
		//UIStatusEnum = UIVisibility::Visible;
	}
	/*
	    DrawDebugLine(GetWorld(), StartLocation, FirstCheckPoint, FColor::Red, false, 5.f, TraceChannel, 1.f);
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Blue, false, 5.f, TraceChannel, 1.f);
		DrawDebugLine(GetWorld(), FirstCheckPoint, SecondCheckPoint, FColor::Red, false, 5.f, TraceChannel, 1.f);
		//DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Blue, false, 5.f, TraceChannel, 1.f);
		DrawDebugLine(GetWorld(), SecondCheckPoint, EndLocation, FColor::Red, false, 5.f, TraceChannel, 1.f);
		DrawDebugBox(GetWorld(), Hit.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Blue, false, 5.f, TraceChannel, 1.f);
		for (FHitResult hit : Hits)
		{
			DrawDebugBox(GetWorld(), hit.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Blue, false, 5.f, TraceChannel, 1.f);
		}
	*/
	
}
