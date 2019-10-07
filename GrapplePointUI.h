// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "GrapplePointUI.generated.h"

/**
 * 
 */
UCLASS()
class ZENITH_API UGrapplePointUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UGrapplePointUI(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image")
	class UImage *GrapplePointUIImage;

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetVisible();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetUnVisible();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetReachable();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetUnReachable();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetTargeting();

	UFUNCTION(BlueprintCallable, Category = "UI Setting")
	void SetUnTargeting();
	
};
