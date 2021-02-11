// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPawn.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "DrawDebugHelpers.h"
#include "Math/UnrealMathUtility.h"
#include "CustomGround.h"
#include "CustomShape.h"
#include "TACCollisionSystemModule/Public//TACCollisionSystem.h"

static TWeakObjectPtr<UTACCollisionSystem> CollisionSystem;

static TWeakObjectPtr<ACustomShape> DraggableActor;
static FTACBaseCollider* DraggableCollider = nullptr;

static float X, Y;
static float SavedMouseX, SavedMouseY;
static bool bIsDragging = false;

static bool bIsMoving = false;
static FVector SavedLocation;

float AngleBetweenVectors(const FVector& V1, const FVector& V2)
{
	return FMath::Acos(FVector::DotProduct(V1, V2) / (V1.Size() * V2.Size()));
}


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

	CollisionSystem = GetWorld()->GetSubsystem<UTACCollisionSystem>();
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
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + WorldDirection * 10000.0f, FColor::Green, false, 10.0f);

	DraggableActor.Reset();
	DraggableCollider = nullptr;

	FTACCollisionResult LinecastResult;
	if (CollisionSystem->LineTrace(WorldLocation, WorldDirection, LinecastResult))
	{
		UObject* HitObject = LinecastResult.GetHitActor().GetObject();
		UE_LOG(LogTemp, Warning, TEXT("HitActor = {%s}. Location = {%s}."), *HitObject->GetName(), *LinecastResult.GetHitPoint().ToString());

		DrawDebugSphere(GetWorld(), LinecastResult.GetHitPoint(), 5.0f, 15.0f, FColor::Red, false, 5.0f);

		if (HitObject->IsA(ACustomShape::StaticClass())) // TO DO IDraggable Interface.
		{
			DraggableActor = Cast<ACustomShape>(HitObject);
			PlayerController->CurrentMouseCursor = EMouseCursor::GrabHandClosed;
			PlayerController->GetMousePosition(SavedMouseX, SavedMouseY);

			if (LinecastResult.GetHitCollider() != nullptr)
			{
				PlayerController->bShowMouseCursor = false;
				DraggableCollider = LinecastResult.GetHitCollider();
				bIsDragging = true;
			}
			else
			{
				const FVector& TableLocation = DraggableActor->GetCustomTransform().Location;
				const float Height = abs(GetActorLocation().Z - TableLocation.Z);

				const FVector PlaneProjection(WorldDirection.X, WorldDirection.Y, 0.0f);
				const float AngleBetweenTableAndPoint = AngleBetweenVectors(WorldDirection, PlaneProjection);
				const float Hypotenuse = Height / FMath::Sin(AngleBetweenTableAndPoint);
				SavedLocation = GetActorLocation() + (WorldDirection * Hypotenuse);

				bIsMoving = true;
			}
		}
		else if (HitObject->IsA(ACustomGround::StaticClass()))
		{
			ACustomShape::Create(GetWorld(), LinecastResult.GetHitPoint());
		}
	}
}

void ACustomPawn::EndDrag()
{
	PlayerController->CurrentMouseCursor = EMouseCursor::Default;

	if (bIsDragging)
	{
		PlayerController->bShowMouseCursor = true;
		bIsDragging = false;

		if (!DraggableActor.IsValid() || DraggableCollider == nullptr || !DraggableCollider->GetHittableActor().IsValid())
		{
			return;
		}

		DraggableActor->ResetDraggableSphere();
	}

	if (bIsMoving)
	{
		bIsMoving = false;
	}
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

void ACustomPawn::Move() const
{
	if (!DraggableActor.IsValid())
	{
		return;
	}
	
	FVector WorldLocation, WorldDirection;
	PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	const FVector& TableLocation = DraggableActor->GetCustomTransform().Location;
	const float Height = abs(GetActorLocation().Z - TableLocation.Z);

	const FVector PlaneProjection(WorldDirection.X, WorldDirection.Y, 0.0f);
	const float AngleBetweenTableAndPoint = AngleBetweenVectors(WorldDirection, PlaneProjection);
	const float Hypotenuse = Height / FMath::Sin(AngleBetweenTableAndPoint);
	const FVector NewTableLocation = GetActorLocation() + (WorldDirection * Hypotenuse);

	//FlushPersistentDebugLines(GetWorld());
	////UE_LOG(LogTemp, Warning, TEXT("Center %s"), *NewTableLocation.ToString());
	//DrawDebugSphere(GetWorld(), SavedLocation, 5.0f, 15, FColor::Green, true);
	//DrawDebugSphere(GetWorld(), NewTableLocation, 5.0f, 15, FColor::Red, true);
	//DrawDebugSphere(GetWorld(), TableLocation, 5.0f, 15, FColor::Black, true);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), 5.0f, 15, FColor::Magenta, true);

	//// HEIGHT PROJECTION
	//const FVector HeightVector = GetActorLocation() + FVector::DownVector * Height;
	//DrawDebugLine(GetWorld(), GetActorLocation(), HeightVector, FColor::Yellow, true);	

	//// MOUSE PROJECTION DIR
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + WorldDirection  * 5000, FColor::Green, true);	

	//// MOUSE PLANE PROJECTION DIR 
	//DrawDebugLine(GetWorld(),HeightVector, HeightVector + PlaneProjection * 5000, FColor::Red, true);		

	if (FVector::Distance(SavedLocation, NewTableLocation) < 5.0f)
	{
		return;
	}

	const FVector TableMoveDir = NewTableLocation - SavedLocation;
	if (DraggableActor->Move(TableMoveDir.GetSafeNormal() * TableMoveDir.Size()))
	{
		SavedLocation = NewTableLocation;
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

	if (bIsMoving)
	{
		Move();
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