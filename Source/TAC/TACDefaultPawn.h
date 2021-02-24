// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TAC/TAC.h"
#include "GameFramework/DefaultPawn.h"
#include "TACDefaultPawn.generated.h"

UCLASS()
class TAC_API ATACDefaultPawn : public ADefaultPawn
{
	GENERATED_BODY()

public:
	ATACDefaultPawn(const FObjectInitializer& ObjectInitializer);

public:
	void SprintForward(float Value);

	void PanRight(float Value);
	void PanUp(float Value);

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	float SprintSpeed;

	UPROPERTY(EditAnywhere)
	float PanSpeed;
};
