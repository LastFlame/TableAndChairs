// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TAC/TAC.h"
#include "GameFramework/PlayerController.h"
#include "TACPlayerController.generated.h"

class ATACDefaultPawn;

UCLASS()
class TAC_API ATACDefaultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

protected:
	// Begin PlayerController overrides 
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	// End PlayerController overrides 

protected:
	// Begin Binded input methods 
	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void MoveUp(float Value);

	UFUNCTION()
	void Turn(float Value);

	UFUNCTION()
	void TurnAtRate(float Value);

	UFUNCTION()
	void LookUp(float Value);

	UFUNCTION()
	void LookUpAtRate(float Value);

	UFUNCTION()
	void PanRight(float Value);

	UFUNCTION()
	void PanUp(float Value);

	UFUNCTION()
	void SprintForward(float Value);

	UFUNCTION()
	void ShootLinecast();

	UFUNCTION()
	void EndHitActorDrag();

	UFUNCTION()
	void ToggleRotation();

	UFUNCTION()
	void TogglePanning();

	UFUNCTION()
	void DestoyHitActor();
	// End Binded input methods

private:
	void DragHitActor();
	void MoveHitActor() const;

protected:
	UPROPERTY()
	ATACDefaultPawn* TACDefaultPossessedPawn;

	bool bRotationEnabled = true;
	bool bPaddingEnabled = false;
};
