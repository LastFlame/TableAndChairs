// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/DefaultPawn.h"
#include "CustomPawn.generated.h"

UCLASS()
class TAC_API ACustomPawn : public ADefaultPawn
{
	GENERATED_BODY()

public:
	ACustomPawn();

protected:
	virtual void BeginPlay() override;

	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;

	virtual void MoveRight(float Val) override;
	virtual void MoveUp_World(float Val) override;

private:
	UFUNCTION()
	void ShootRaycast();

	UFUNCTION()
	void EndDrag();

	UFUNCTION()
	void ToggleRotation();

	UFUNCTION()
	void TogglePanning();

	UFUNCTION()
	void PanRight(float Value);

	UFUNCTION()
	void PanUp(float Value);

	UFUNCTION()
	void DestroySelectedActor();

	void Drag() const;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere)
	float PanSpeed;

	UPROPERTY()
	class ACustomShape* DraggableActor;

	struct FCustomRaycastBaseCollider* DraggableCollider;

	FVector PanVector;

	APlayerController* PlayerController;
	bool bRotating;
	bool bPanning;
};
