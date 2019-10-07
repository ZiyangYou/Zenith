// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZenithGameMode.generated.h"

UCLASS(minimalapi)
class AZenithGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AZenithGameMode(const FObjectInitializer & ObjectInitializer);

	UPROPERTY()
		TArray<class AEnemyAbstract*> ListOfAllEnemies;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool CheckIsEnemiesInAttackState();

	UFUNCTION(BlueprintCallable)
	void DelayBeforeCheckForEnemiesInAttackState(float Time);

	UFUNCTION(BlueprintCallable)
	void RemoveEnemyFromTheList(AEnemyAbstract * Enemy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		float ResetMusicTimer;

	UPROPERTY()
		class AZenithGameState *CurrentGameState;

protected:

	float OutBattleTimer;

};



