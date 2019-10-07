// Fill out your copyright notice in the Description page of Project Settings.

#include "GrapplePointUI.h"
#include "Image.h"

UGrapplePointUI::UGrapplePointUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UGrapplePointUI::NativeConstruct()
{
	Super::NativeConstruct();

}

void UGrapplePointUI::SetVisible()
{
	
		GrapplePointUIImage->SetColorAndOpacity(FLinearColor::White);
}

void UGrapplePointUI::SetUnVisible()
{
		GrapplePointUIImage->SetColorAndOpacity(FLinearColor::Transparent);
}

void UGrapplePointUI::SetReachable()
{
		GrapplePointUIImage->SetColorAndOpacity(FLinearColor::Yellow);
}

void UGrapplePointUI::SetUnReachable()
{
		GrapplePointUIImage->SetColorAndOpacity(FLinearColor::White);
}

void UGrapplePointUI::SetTargeting()
{
		GrapplePointUIImage->SetColorAndOpacity(FLinearColor::Green);
}

void UGrapplePointUI::SetUnTargeting()
{
	GrapplePointUIImage->SetColorAndOpacity(FLinearColor::Yellow);
}

