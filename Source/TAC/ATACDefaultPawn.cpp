// Fill out your copyright notice in the Description page of Project Settings.


#include "TAC/ATACDefaultPawn.h"

static FVector MovementVector;

ATACDefaultPawn::ATACDefaultPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer), SprintSpeed(30.0f), PanSpeed(10.0f)
{
	bAddDefaultMovementBindings = false;
}

void ATACDefaultPawn::SprintForward(float Value)
{
	if (Value != 0)
	{
		const FRotator ControlRotator = Controller->GetControlRotation();
		MovementVector = FRotationMatrix(ControlRotator).GetScaledAxis(EAxis::X) * (Value * SprintSpeed);
	}
}


void ATACDefaultPawn::PanRight(float Value)
{
	if (Value != 0)
	{
		const FRotator ControlRotator = Controller->GetControlRotation();
		MovementVector += FRotationMatrix(ControlRotator).GetScaledAxis(EAxis::Y) * Value * PanSpeed;
	}
}

void ATACDefaultPawn::PanUp(float Value)
{
	if (Value != 0)
	{
		MovementVector += FVector::UpVector * -Value * PanSpeed;
	}
}

void ATACDefaultPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (MovementVector.Size() != 0.0f)
	{
		AddActorWorldOffset(MovementVector);
		MovementVector = FVector::ZeroVector;
	}
}