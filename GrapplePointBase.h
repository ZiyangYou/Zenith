// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrapplePointBase.generated.h"



UENUM(BlueprintType)
enum class GrapplePointStatus: uint8
{
	OutRange UMETA(DisplayName = "OutRange"),
	Visible UMETA(DisplayName = "Visible"),
	Reachable UMETA(DisplayName = "Reachable"),
	Targeting UMETA(DisplayName = "Targeting")
};

UENUM(BlueprintType)
enum class UIVisibility : uint8
{
	Visible UMETA(DisplayName = "Visible"),
	UnVisible UMETA(DisplayName = "UnVisible")
};

UCLASS()
class ZENITH_API AGrapplePointBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrapplePointBase();

	UFUNCTION(BlueprintCallable, Category = "GrapplePoint Setting")
	void SetIsTargeting();

	UFUNCTION(BlueprintCallable, Category = "GrapplePoint Setting")
	void SetIsNotTargeting();

	UFUNCTION(BlueprintCallable, Category = "GrapplePoint Setting")
	virtual void SetIsVisible();

	UFUNCTION(BlueprintCallable, Category = "GrapplePoint Setting")
	virtual void SetIsNotVisible();

	UFUNCTION(BlueprintCallable, Category = "GrapplePoint Setting")
	virtual void SetIsReachable();

	UFUNCTION(BlueprintCallable, Category = "GrapplePoint Setting")
	virtual void SetIsNotReachable();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetUIBlocked();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetUINotBlocked();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetPlayerOnTop();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetPlayerNotOnTop();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	bool GetIsPlayerOnTop();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	UIVisibility GetUIStatus();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UIVisibility UIStatusEnum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DotProduction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UICollisionCheck();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsPlayerOnTop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CapsuleCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	GrapplePointStatus GrappleStatusEnum;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent *GrappleUI;

	UPROPERTY(EditAnywhere, Category = "Widgets")
	TSubclassOf<class UUserWidget>GrappleHookUI;

	UFUNCTION(BlueprintCallable, Category = "GrapplePoint Status")
	GrapplePointStatus GetGrappleStatus();

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

};
