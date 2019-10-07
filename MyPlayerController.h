// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CubbyBase.h"
#include "MyPlayerController.generated.h"

/**
 *
 */
UCLASS()
class ZENITH_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:

	

};
