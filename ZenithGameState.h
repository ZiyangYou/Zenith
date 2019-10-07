// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BackgroundMusicInterface.h"
#include "ZenithGameState.generated.h"

/**
 * 
 */
UCLASS()
class ZENITH_API AZenithGameState : public AGameStateBase, public IBackgroundMusicInterface 
{
	GENERATED_BODY()

public:

	AZenithGameState(const FObjectInitializer & ObjectInitializer);

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
		class UAudioComponent* AudioComponentPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundCue* ExplorationCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		class USoundCue* BattleCue;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Music")
		void ChangeMusic(int MusicIndex);
	virtual void ChangeMusic_Implementation(int MusicIndex);

	UFUNCTION()
	void PlayExplorationMusic();

	UFUNCTION()
	void GetCurrentAudioPlaybackPercentage(const class USoundWave * PlayingSoundWave, const float PlaybackPercent);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		float FadeOutTime;

	class UAudioComponent* AudioComponent;

	UFUNCTION(BlueprintCallable)
		void addOneEnemyInAttackMode();

	UFUNCTION(BlueprintCallable)
		void removeOneEnemyInAttackMode();

	UFUNCTION(BlueprintCallable)
		int GetTheNumOfEnemiesInAttackMode();


protected:

	float CurrentSoundPlayTime;

	int CurrentNumberOfEnemiesInAttackMode;

};
