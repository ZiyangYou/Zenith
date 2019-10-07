// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ZenithCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Math/UnrealMathUtility.h"
#include "App.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AZenithCharacter

AZenithCharacter::AZenithCharacter(const FObjectInitializer& ObjectInitializer):ACharacter(ObjectInitializer)
{

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//audio component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	AudioComponent->SetupAttachment(RootComponent);

	IsWalking = false;
	IsStunned = false;
	IsInterruptable = true;
	IsAlive = true;
	IsInvincible = false;
	turn = 0;
	IsMovingRight = false;
	IsMovingLeft = false;
	CanMoveCamera = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AZenithCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AZenithCharacter::Walk);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AZenithCharacter::StopWalk);

	PlayerInputComponent->BindAxis("MoveForward", this, &AZenithCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AZenithCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AZenithCharacter::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &AZenithCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AZenithCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AZenithCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AZenithCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AZenithCharacter::TouchStopped);

	// dodge
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AZenithCharacter::Dodge);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AZenithCharacter::OnResetVR);
}

void AZenithCharacter::Walk()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	IsWalking = true;
	//UE_LOG(LogTemp, Warning, TEXT("SpeedMultipler is:  %f"), SpeedMultipler)
}

void AZenithCharacter::StopWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	IsWalking = false;
	//UE_LOG(LogTemp, Warning, TEXT("SpeedMultipler is:  %f"), SpeedMultipler)
}

void AZenithCharacter::ResetHealth()
{
	CurrentHealth = TotalHealth;
	HealthPercentage = CurrentHealth / TotalHealth;
}

void AZenithCharacter::Jump()
{
	if (IsStunned) return;
	Super::Jump();

	if (JumpAudio && !GetCharacterMovement()->IsFalling())
	{
		AudioComponent->SetSound(JumpAudio);
		AudioComponent->Play();
	}

}

void AZenithCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Set up player status
	CurrentHealth = TotalHealth;
	HealthPercentage = CurrentHealth / TotalHealth;

	//Animation 
	AnimInstance = GetMesh()->GetAnimInstance();
}

void AZenithCharacter::SetIsStunned()
{
	IsStunned = true;
}

void AZenithCharacter::SetIsNotStunned()
{
	IsStunned = false;
}

float AZenithCharacter::GetHealthPercentage()
{
	return HealthPercentage;
}

float AZenithCharacter::GetIsAlive()
{
	return IsAlive;
}

void AZenithCharacter::OnHurt(float dmg, float KnockBackForceRatio)
{
	if (!IsInvincible)
	{
		CurrentHealth -= dmg;
		HealthPercentage = CurrentHealth / TotalHealth;

		//play audio
		if (dmg > 0 && IsInterruptable)
		{
			if (HurtAudio && dmg >= 5)
			{
				AudioComponent->SetSound(HurtAudio);
				AudioComponent->Play();
			}
			if (AnimInstance && HurtAnim)
			{
				//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
				AnimInstance->Montage_Play(HurtAnim, 1.2f);
			}
			IsStunned = true;
		}
	}
}

void AZenithCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AZenithCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AZenithCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AZenithCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (CanMoveCamera)
	{
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		turn = Rate;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Turn initial! : %f"), turn)
}

void AZenithCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (CanMoveCamera)
	{
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void AZenithCharacter::Turn(float Value)
{
	if (CanMoveCamera)
	{
		AddControllerYawInput(Value);
		turn = Value;
	}
}

void AZenithCharacter::LookUp(float Value)
{
	if (CanMoveCamera)
	{
		AddControllerPitchInput(Value);
	}
}

void AZenithCharacter::MoveForward(float Value)
{
	if (Value > 0.1)
	{
		IsMovingForward = true;
		IsMovingBackward = false;
	}
	else if (Value < -0.1)
	{
		IsMovingBackward = true;
		IsMovingForward = false;
	}
	else
	{
		IsMovingForward = false;
		IsMovingBackward = false;
	}
	if ((Controller != NULL) && (Value != 0.0f) && !IsStunned)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		//UE_LOG(LogTemp, Warning, TEXT("SpeedMultipler is:  %f"), SpeedMultipler)
	}
	float InterSpeed;
	float TargetAmount;
	FMath::Clamp<float>(turn, -1, 1);
	//UE_LOG(LogTemp, Warning, TEXT("Turn Clamp! : %f"), turn)
	if (GetCharacterMovement()->IsFalling() || IsWalking)
	{
		TargetAmount = 0;
		InterSpeed = 10;
	}
	else
	{
		TargetAmount = turn;
		InterSpeed = 1;
	}

	FMath::FInterpTo(PlayerLeanAmount, TargetAmount, FApp::GetDeltaTime(), InterSpeed);
}

void AZenithCharacter::MoveRight(float Value)
{
	if (Value > 0.1)
	{
		IsMovingRight = true;
		IsMovingLeft = false;
	}
	else if (Value < -0.1)
	{
		IsMovingLeft = true;
		IsMovingRight = false;
	}
	else
	{
		IsMovingLeft = false;
		IsMovingRight = false;
	}
	if ( (Controller != NULL) && (Value != 0.0f) && !IsStunned)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AZenithCharacter::Dodge()
{
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();

	LaunchCharacter(ForwardVector * 100, true, true);

}