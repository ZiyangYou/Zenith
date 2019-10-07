// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "GameFramework/Character.h"
#include "ZenithCharacter.generated.h"

UCLASS(config=Game)
class AZenithCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	AZenithCharacter(const FObjectInitializer&);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	bool IsWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	bool IsInvincible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		bool IsInterruptable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool IsMovingRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool IsMovingLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool IsMovingForward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		bool IsMovingBackward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		bool CanMoveCamera;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void SetIsStunned();

	UFUNCTION(BlueprintCallable)
		void SetIsNotStunned();

	UFUNCTION(BlueprintCallable)
		float GetHealthPercentage();

	UFUNCTION(BlueprintCallable)
		float GetIsAlive();


protected:

	// set up variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		float TotalHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundCue* DeathAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundCue* HurtAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
		USoundCue* JumpAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		bool IsStunned;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		bool IsAlive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		float HealthPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		float MeleeAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		float RangeAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		float RangeAttackRange;

	//Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Animation")
		class UAnimMontage *DeathAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Animation")
		class UAnimMontage *HurtAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Perception Sight")
	UAnimInstance *AnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Perception Sight")
		float PlayerLeanAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterStatus)
		float WalkingSpeed;
	
	UPROPERTY()
		float turn;

	//function is called when character gets hurt

	virtual void OnHurt(float dmg, float KnockBackForceRatio);

	virtual void Jump() override;

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	// called for dodge
	void Dodge();

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void Turn(float Value);

	void LookUp(float Value);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Walk();
	
	void StopWalk();

	UFUNCTION(BlueprintCallable)
	void ResetHealth();

	// End of APawn interface
};

