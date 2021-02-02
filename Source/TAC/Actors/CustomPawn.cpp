// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPawn.h"
#include "GameFramework/PlayerInput.h"
#include "CustomGround.h"
#include "CustomShape.h"
#include "DrawDebugHelpers.h"
#include "TAC/CustomCollisions/CustomCollisionSystem.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "TAC/CustomShapeTemplateDataAsset.h"
#include "TAC/CustomGameMode.h"

static TWeakObjectPtr<ACustomShape> DraggableActor;
static FCustomBaseCollider* DraggableCollider = nullptr;

static float X, Y;
static float SavedMouseX, SavedMouseY;
static bool bIsDragging = false;

ACustomPawn::ACustomPawn() : PanSpeed(5.0f), bRotating(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACustomPawn::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<APlayerController>(GetController());
	check(PlayerController);

	PlayerController->bShowMouseCursor = !bRotating;
}

void ACustomPawn::AddControllerYawInput(float Val)
{
	if (bRotating)
	{
		Super::AddControllerYawInput(Val);
	}
}

void ACustomPawn::AddControllerPitchInput(float Val)
{
	if (bRotating)
	{
		Super::AddControllerPitchInput(Val);
	}
}

void ACustomPawn::MoveRight(float Val)
{
	if (!bPanning)
	{
		Super::MoveRight(Val);
	}
}

void ACustomPawn::MoveUp_World(float Val)
{
	if (!bPanning)
	{
		Super::MoveUp_World(Val);
	}
}

void ACustomPawn::ShootRaycast()
{
	FVector WorldLocation, WorldDirection;
	PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	CustomCollisionSystem::FCustomCollisionResult LinecastResult;

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + WorldDirection * 10000.0f, FColor::Green, false, 10.0f);

	DraggableActor.Reset();
	DraggableCollider = nullptr;

	if (CustomCollisionSystem::LineTrace(WorldLocation, WorldDirection, LinecastResult))
	{
		UObject* HitObject = LinecastResult.GetHitActor().GetObject();
		UE_LOG(LogTemp, Warning, TEXT("HitActor = {%s}. Location = {%s}."), *HitObject->GetName(), *LinecastResult.GetHitPoint().ToString());

		DrawDebugSphere(GetWorld(), LinecastResult.GetHitPoint(), 5.0f, 15.0f, FColor::Red, false, 5.0f);

		if (HitObject->IsA(ACustomShape::StaticClass())) // TO DO IDraggable Interface.
		{
			PlayerController->GetMousePosition(SavedMouseX, SavedMouseY);
			PlayerController->bShowMouseCursor = false;

			DraggableActor = Cast<ACustomShape>(HitObject);
			DraggableCollider = LinecastResult.GetHitCollider();

			bIsDragging = true;
		}
		else if (HitObject->IsA(ACustomGround::StaticClass()))
		{
			const ACustomGameMode* CustomGameMode = Cast<const ACustomGameMode>(GetWorld()->GetAuthGameMode());
			if (CustomGameMode == nullptr)
			{
				return;
			}

			const UCustomShapeTemplateDataAsset& CustomShapeTemplateData = CustomGameMode->GetCustomShapeTemplateData();

			FCustomBoxCollider TableToSpawnCollider;
			TableToSpawnCollider.SetMinBounds(LinecastResult.GetHitPoint() + CustomShapeTemplateData.GetDefaultTableBoundCollider().GetMinBounds());
			TableToSpawnCollider.SetMaxBounds(LinecastResult.GetHitPoint() + CustomShapeTemplateData.GetDefaultTableBoundCollider().GetMaxBounds());

			CustomCollisionSystem::FCustomCollisionResult Result;
			if (!CustomCollisionSystem::BoxTrace(TableToSpawnCollider, ECustomCollisionFlags::Static, Result))
			{
				ACustomShape::Create(GetWorld(), LinecastResult.GetHitPoint());
			}
		}
	}
}

void ACustomPawn::EndDrag()
{
	PlayerController->bShowMouseCursor = true;
	bIsDragging = false;
}

void ACustomPawn::Drag() const
{
	if (!DraggableActor.IsValid() || DraggableCollider == nullptr || !DraggableCollider->GetHittableActor().IsValid())
	{
		return;
	}

	FVector WorldLocation, WorldDirection;
	PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	const float ColliderToMouseProjDist = FVector::Distance(DraggableCollider->GetLocation(), WorldLocation);
	WorldLocation += WorldDirection * ColliderToMouseProjDist;
	WorldLocation.Z = DraggableCollider->GetLocation().Z;

	if (DraggableActor->Drag(*DraggableCollider, WorldLocation))
	{
		FVector2D MouseLoc;
		PlayerController->ProjectWorldLocationToScreen(DraggableCollider->GetLocation(), MouseLoc);
		PlayerController->SetMouseLocation(MouseLoc.X, MouseLoc.Y);
	}
}

void ACustomPawn::ToggleRotation()
{
	bRotating = !bRotating;

	PlayerController->bShowMouseCursor = !bRotating;

	if (bRotating)
	{
		PlayerController->GetMousePosition(X, Y);	
	}
}

void ACustomPawn::TogglePanning()
{
	bPanning = !bPanning;

	PlayerController->bShowMouseCursor = !bPanning;

	if (bPanning)
	{
		PlayerController->GetMousePosition(X, Y);
	}
}

void ACustomPawn::PanRight(float Value)
{
	if (bPanning)
	{
		const FRotator ControlRotator = PlayerController->GetControlRotation();
		PanVector += FRotationMatrix(ControlRotator).GetScaledAxis(EAxis::Y) * Value * PanSpeed;
	}
}


void ACustomPawn::PanUp(float Value)
{
	if (bPanning)
	{
		const FRotator ControlRotator = PlayerController->GetControlRotation();
		
		PanVector += FRotationMatrix(ControlRotator).GetScaledAxis(EAxis::Z) * -Value * PanSpeed;
	}
}

void ACustomPawn::DestroySelectedActor()
{
	if (bIsDragging || !DraggableActor.IsValid())
	{
		return;
	}

	GetWorld()->DestroyActor(DraggableActor.Get());
	DraggableCollider = nullptr;
}

void ACustomPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PanVector.Size() != 0.0f)
	{
		AddActorLocalOffset(PanVector);
		PanVector = FVector::ZeroVector;
	}

	if (bRotating || bPanning)
	{
		PlayerController->SetMouseLocation((int)X, (int)Y);
	}

	if (bIsDragging)
	{
		Drag();
	}
}

void InitializeCustomPawnInputBindings()
{
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomPawn_LeftClick", EKeys::LeftMouseButton));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomPawn_RightClick", EKeys::RightMouseButton));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustomPawn_MiddleClick", EKeys::MiddleMouseButton));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CustoPawn_Delete", EKeys::Delete));
}

void ACustomPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	InitializeCustomPawnInputBindings();

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("CustomPawn_LeftClick", EInputEvent::IE_Pressed, this, &ACustomPawn::ShootRaycast);
	PlayerInputComponent->BindAction("CustomPawn_LeftClick", EInputEvent::IE_Released, this, &ACustomPawn::EndDrag);

	PlayerInputComponent->BindAction("CustomPawn_RightClick", EInputEvent::IE_Pressed, this, &ACustomPawn::ToggleRotation);
	PlayerInputComponent->BindAction("CustomPawn_RightClick", EInputEvent::IE_Released, this, &ACustomPawn::ToggleRotation);

	PlayerInputComponent->BindAction("CustomPawn_MiddleClick", EInputEvent::IE_Pressed, this, &ACustomPawn::TogglePanning);
	PlayerInputComponent->BindAction("CustomPawn_MiddleClick", EInputEvent::IE_Released, this, &ACustomPawn::TogglePanning);

	PlayerInputComponent->BindAction("CustoPawn_Delete", EInputEvent::IE_Pressed, this, &ACustomPawn::DestroySelectedActor);

	PlayerInputComponent->BindAxis("DefaultPawn_Turn", this, &ACustomPawn::PanRight);
	PlayerInputComponent->BindAxis("DefaultPawn_LookUp", this, &ACustomPawn::PanUp);
}