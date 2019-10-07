// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ZenithGameMode.h"
#include "ZenithCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAbstract.h"
#include "EnemyController.h"
#include "Engine/Public/TimerManager.h"
#include "Engine/World.h"
#include "ZenithGameState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"

AZenithGameMode::AZenithGameMode(const FObjectInitializer& ObjectInitializer) : AGameModeBase(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	/*
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Class'/Game/Character/Player/BP_CH_Su.BP_CH_Su_C'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	*/
	ListOfAllEnemies = TArray<AEnemyAbstract*>();
	ResetMusicTimer = 4;
	PrimaryActorTick.bCanEverTick = true;
}

void AZenithGameMode::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AEnemyAbstract> It(GetWorld()); It; ++It)
	{
		ListOfAllEnemies.Add(*It);
	}

	CurrentGameState = Cast<AZenithGameState>(GetWorld()->GetGameState());
}

void AZenithGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	if (CurrentGameState && !CheckIsEnemiesInAttackState())
	{
		CurrentGameState->ChangeMusic(0);
	}
	*/

	/*
	if (!CheckIsEnemiesInAttackState())
	{
		OutBattleTimer += DeltaTime;
		if (OutBattleTimer >= ResetMusicTimer)
		{
			if (CurrentGameState)
			{
				CurrentGameState->ChangeMusic(0);
			}
		}
	}
	else 
	{
		OutBattleTimer = 0;
		if (CurrentGameState)
		{
			CurrentGameState->ChangeMusic(1);
		}
	}
	*/
}

bool AZenithGameMode::CheckIsEnemiesInAttackState()
{
	bool result = false;
	for (AEnemyAbstract* tmp : ListOfAllEnemies)
	{
		if (!tmp) continue;
		if (!tmp->IsValidLowLevel()) continue;

		if(tmp)
		{
			AEnemyController* AIController = Cast<AEnemyController>(tmp->GetController());
			if (AIController)
			{
				if (AIController->GetCurrentAIState() != uint8(AIStates::Patrolling))
				{
					result = true;
					break;
				}
				
			}
		}
	}
	return result;
}

void AZenithGameMode::DelayBeforeCheckForEnemiesInAttackState(float Time)
{
	//FTimerHandle UnusedHandle;
	//GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AZenithGameMode::CheckIsEnemiesInAttackState, Time, false);
}

void AZenithGameMode::RemoveEnemyFromTheList(AEnemyAbstract* Enemy)
{
	if (Enemy && ListOfAllEnemies.Contains(Enemy))
	{
		ListOfAllEnemies.Remove(Enemy);
	}
}



