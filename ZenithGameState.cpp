// Fill out your copyright notice in the Description page of Project Settings.

#include "ZenithGameState.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"
#include "Engine/Public/TimerManager.h"

AZenithGameState::AZenithGameState(const FObjectInitializer& ObjectInitializer) : AGameStateBase(ObjectInitializer)
{
	AudioComponentPtr = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("Audio Component"));
	AudioComponent = AudioComponentPtr;

	CurrentNumberOfEnemiesInAttackMode = 0;
}

void AZenithGameState::BeginPlay()
{
	Super::BeginPlay();

	if (AudioComponent)
	{

		AudioComponent->SetSound(ExplorationCue);
		AudioComponent->Play();

		AudioComponent->OnAudioPlaybackPercent.AddDynamic(this, &AZenithGameState::GetCurrentAudioPlaybackPercentage);
	}

}

void AZenithGameState::ChangeMusic_Implementation(int MusicIndex)
{
	switch (MusicIndex)
	{
	default:
		break;
	case 0:
		if (ExplorationCue && AudioComponent->Sound != ExplorationCue) 
		{
			AudioComponent->FadeOut(FadeOutTime, 0);
			FTimerHandle UnusedHandle;
			GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AZenithGameState::PlayExplorationMusic, FadeOutTime, false);
		}
		break;
	case 1:
		if (BattleCue && AudioComponent->Sound != BattleCue)
		{
			AudioComponent->SetSound(BattleCue);
			AudioComponent->Play(CurrentSoundPlayTime);
		}
		break;
	}
}

void AZenithGameState::PlayExplorationMusic()
{
	if (AudioComponent && ExplorationCue)
	{
		AudioComponent->SetSound(ExplorationCue);
		AudioComponent->FadeIn(2, 1, CurrentSoundPlayTime + FadeOutTime);
	}
}

void AZenithGameState::GetCurrentAudioPlaybackPercentage(const class USoundWave* PlayingSoundWave, const float PlaybackPercent)
{
	CurrentSoundPlayTime = PlayingSoundWave->Duration * PlaybackPercent;
}

void AZenithGameState::addOneEnemyInAttackMode()
{
	CurrentNumberOfEnemiesInAttackMode++;
}

void AZenithGameState::removeOneEnemyInAttackMode()
{
	CurrentNumberOfEnemiesInAttackMode--;
}

int AZenithGameState::GetTheNumOfEnemiesInAttackMode()
{
	return CurrentNumberOfEnemiesInAttackMode;
}
