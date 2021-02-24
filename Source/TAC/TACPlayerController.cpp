// Fill out your copyright notice in the Description page of Project Settings.


#include "TACPlayerController.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerInput.h"
#include "TAC/TACDefaultPawn.h"
#include "TAC/TACShapesManager.h"
#include "TAC/Shapes/TACTableShape.h"
#include "TACCollisionSystemModule/Public//TACCollisionSystem.h"

static TWeakObjectPtr<UTACCollisionSystem> CollisionSystem;
static TWeakObjectPtr<UTACShapesManager> ShapesManager;

static TWeakObjectPtr<ATACTableShape> DraggableActor;
static FTACBaseCollider* DraggableCollider = nullptr;

static float DragSavedMouseX, DragSavedMouseY;
static bool bDraggingTable = false;

static bool bMovingTable = false;
static FVector MovableTableSavedLocation;

static float RotationSavedMouseX, RotationSavedMouseY;
static bool bRotating = false;

static bool bPanning = false;

float VectorsAngle(const FVector& V1, const FVector& V2)
{
	return FMath::Acos(FVector::DotProduct(V1, V2) / (V1.Size() * V2.Size()));
}

FVector GetTableMovedLocation(const FVector& PawnLocation, const FVector& WorldDirection, const FVector& TableLocation)
{
	const float Height = abs(PawnLocation.Z - TableLocation.Z);

	const FVector PlaneProjection(WorldDirection.X, WorldDirection.Y, 0.0f);
	const float AngleBetweenTableAndPoint = VectorsAngle(WorldDirection, PlaneProjection);
	const float Hypotenuse = Height / FMath::Sin(AngleBetweenTableAndPoint);

	return PawnLocation + (WorldDirection * Hypotenuse);
}

void ATACDefaultPlayerController::Tick(float DeltaSeconds)
{
	if (bRotating || bPanning)
	{
		SetMouseLocation(RotationSavedMouseX, RotationSavedMouseY);
	}
	
	if (bDraggingTable)
	{
		DragHitActor();
	}

	if (bMovingTable)
	{
		MoveHitActor();
	}
}

void InitializeDefaultInputBindings(UPlayerInput* PlayerInput)
{
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveForward",		EKeys::W, 1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveForward",		EKeys::S, -1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveForward",		EKeys::Up, 1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveForward",		EKeys::Down, -1.f));

	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveRight",			EKeys::A, -1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveRight",			EKeys::D, 1.f));

	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveUp",				EKeys::LeftControl, -1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveUp",				EKeys::SpaceBar, 1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveUp",				EKeys::C, -1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveUp",				EKeys::E, 1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_MoveUp",				EKeys::Q, -1.f));

	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_TurnRate",			EKeys::Left, -1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_TurnRate",			EKeys::Right, 1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_Turn",				EKeys::MouseX, 1.f));

	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_LookUpRate",			EKeys::Gamepad_RightY, 1.f));
	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_LookUp",				EKeys::MouseY, -1.f));

	PlayerInput->AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TAC_SprintForward",		EKeys::MouseWheelAxis, 1.f));

	PlayerInput->AddEngineDefinedActionMapping(FInputActionKeyMapping("TAC_LeftClick",		EKeys::LeftMouseButton));
	PlayerInput->AddEngineDefinedActionMapping(FInputActionKeyMapping("TAC_RightClick",		EKeys::RightMouseButton));
	PlayerInput->AddEngineDefinedActionMapping(FInputActionKeyMapping("TAC_MiddleClick",	EKeys::MiddleMouseButton));
	PlayerInput->AddEngineDefinedActionMapping(FInputActionKeyMapping("TAC_Delete",			EKeys::Delete));

}

void ATACDefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InitializeDefaultInputBindings(PlayerInput);

	InputComponent->BindAxis("TAC_MoveForward",		this, &ATACDefaultPlayerController::MoveForward);
	InputComponent->BindAxis("TAC_MoveRight",		this, &ATACDefaultPlayerController::MoveRight);
	InputComponent->BindAxis("TAC_MoveUp",			this, &ATACDefaultPlayerController::MoveUp);
	InputComponent->BindAxis("TAC_Turn",			this, &ATACDefaultPlayerController::Turn);
	InputComponent->BindAxis("TAC_TurnRate",		this, &ATACDefaultPlayerController::TurnAtRate);
	InputComponent->BindAxis("TAC_LookUp",			this, &ATACDefaultPlayerController::LookUp);
	InputComponent->BindAxis("TAC_LookUpRate",		this, &ATACDefaultPlayerController::LookUpAtRate);
	InputComponent->BindAxis("TAC_Turn",			this, &ATACDefaultPlayerController::PanRight);
	InputComponent->BindAxis("TAC_LookUp",			this, &ATACDefaultPlayerController::PanUp);
	InputComponent->BindAxis("TAC_SprintForward",	this, &ATACDefaultPlayerController::SprintForward);

	InputComponent->BindAction("TAC_LeftClick",		EInputEvent::IE_Pressed,	this, &ATACDefaultPlayerController::ShootLinecast);
	InputComponent->BindAction("TAC_LeftClick",		EInputEvent::IE_Released,	this, &ATACDefaultPlayerController::EndHitActorDrag);
	InputComponent->BindAction("TAC_RightClick",	EInputEvent::IE_Pressed,	this, &ATACDefaultPlayerController::ToggleRotation);
	InputComponent->BindAction("TAC_RightClick",	EInputEvent::IE_Released,	this, &ATACDefaultPlayerController::ToggleRotation);
	InputComponent->BindAction("TAC_MiddleClick",	EInputEvent::IE_Pressed,	this, &ATACDefaultPlayerController::TogglePanning);
	InputComponent->BindAction("TAC_MiddleClick",	EInputEvent::IE_Released,	this, &ATACDefaultPlayerController::TogglePanning);
	InputComponent->BindAction("TAC_Delete",		EInputEvent::IE_Pressed,	this, &ATACDefaultPlayerController::DestoyHitActor);
}

void ATACDefaultPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = !bRotating;

	CollisionSystem = GetWorld()->GetSubsystem<UTACCollisionSystem>();
	ShapesManager = GetWorld()->GetSubsystem<UTACShapesManager>();
}

void ATACDefaultPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	TACDefaultPossessedPawn = Cast<ATACDefaultPawn>(InPawn);
}

void ATACDefaultPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	TACDefaultPossessedPawn = nullptr;
}

void ATACDefaultPlayerController::MoveForward(float Value)
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}
	
	TACDefaultPossessedPawn->MoveForward(Value);
}

void ATACDefaultPlayerController::MoveRight(float Value)
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}
	TACDefaultPossessedPawn->MoveRight(Value);
}

void ATACDefaultPlayerController::MoveUp(float Value)
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	TACDefaultPossessedPawn->MoveUp_World(Value);
}

void ATACDefaultPlayerController::Turn(float Value)
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	if (!bRotating)
	{
		return;
	}

	TACDefaultPossessedPawn->AddControllerYawInput(Value);
}

void ATACDefaultPlayerController::TurnAtRate(float Value)
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	TACDefaultPossessedPawn->TurnAtRate(Value);
}

void ATACDefaultPlayerController::LookUp(float Value)
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	if (!bRotating)
	{
		return;
	}

	TACDefaultPossessedPawn->AddControllerPitchInput(Value);
}

void ATACDefaultPlayerController::LookUpAtRate(float Value)
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	TACDefaultPossessedPawn->LookUpAtRate(Value);
}

void ATACDefaultPlayerController::PanRight(float Value)
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	if (!bPanning)
	{
		return;
	}

	TACDefaultPossessedPawn->PanRight(Value);
}

void ATACDefaultPlayerController::PanUp(float Value)
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	if (!bPanning)
	{
		return;
	}

	TACDefaultPossessedPawn->PanUp(Value);
}

void ATACDefaultPlayerController::SprintForward(float Value)
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	TACDefaultPossessedPawn->SprintForward(Value);
}

void ATACDefaultPlayerController::ShootLinecast()
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	const FVector& PawnLocation = GetPawn()->GetActorLocation();

	DrawDebugLine(GetWorld(), PawnLocation, PawnLocation + WorldDirection * 10000.0f, FColor::Green, false, 10.0f);

	DraggableActor.Reset();
	DraggableCollider = nullptr;

	FTACCollisionResult LinecastResult;
	if (!CollisionSystem->LineTrace(WorldLocation, WorldDirection, LinecastResult))
	{
		return;
	}

	TWeakObjectPtr<AActor> HitActor = LinecastResult.GetHitActor()->GetActor();

	UE_LOG(LogTemp, Warning, TEXT("HitActor = {%s}. Location = {%s}."), *HitActor->GetName(), *LinecastResult.GetHitPoint().ToString());
	DrawDebugSphere(GetWorld(), LinecastResult.GetHitPoint(), 5.0f, 15.0f, FColor::Red, false, 5.0f);
		
	if (HitActor->ActorHasTag(ShapesManager->GetShapesTemplateData().GetTableTag()))
	{
		DraggableActor = Cast<ATACTableShape>(HitActor);
		GetMousePosition(DragSavedMouseX, DragSavedMouseY);
		
		CurrentMouseCursor = EMouseCursor::GrabHandClosed;

		if (LinecastResult.GetHitCollider() != nullptr)
		{
			bShowMouseCursor = false;
			DraggableCollider = LinecastResult.GetHitCollider();
			bDraggingTable = true;
		}
		else
		{
			MovableTableSavedLocation = GetTableMovedLocation(PawnLocation, WorldDirection, DraggableActor->GetCustomTransform().Location);
			bMovingTable = true;
		}
	}
	else if (HitActor->ActorHasTag(ShapesManager->GetShapesTemplateData().GetTableSpawnerTag()))
	{
		ShapesManager->CreateTable(LinecastResult.GetHitPoint());
	}
}

void ATACDefaultPlayerController::EndHitActorDrag()
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	CurrentMouseCursor = EMouseCursor::Default;

	if (bDraggingTable)
	{
		bShowMouseCursor = true;
		bDraggingTable = false;

		if (!DraggableActor.IsValid() || DraggableCollider == nullptr || !DraggableCollider->GetHittableActor().IsValid())
		{
			return;
		}

		DraggableActor->ResetDraggableSphere();
	}

	if (bMovingTable)
	{
		bMovingTable = false;
	}
}

void ATACDefaultPlayerController::ToggleRotation()
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	bRotating = !bRotating;

	bShowMouseCursor = !bRotating;

	if (bRotating)
	{
		GetMousePosition(RotationSavedMouseX, RotationSavedMouseY);	
	}
}

void ATACDefaultPlayerController::TogglePanning()
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	bPanning = !bPanning;

	bShowMouseCursor = !bPanning;

	if (bPanning)
	{
		GetMousePosition(RotationSavedMouseX, RotationSavedMouseY);
	}
}

void ATACDefaultPlayerController::DestoyHitActor()
{
	if (!TACDefaultPossessedPawn)
	{
		return;
	}

	if (bDraggingTable || !DraggableActor.IsValid())
	{
		return;
	}

	GetWorld()->DestroyActor(DraggableActor.Get());
	DraggableCollider = nullptr;
}

void ATACDefaultPlayerController::DragHitActor()
{
	if (!DraggableActor.IsValid() || DraggableCollider == nullptr || !DraggableCollider->GetHittableActor().IsValid())
	{
		return;
	}

	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	const float ColliderToMouseProjDist = FVector::Distance(DraggableCollider->GetLocation(), WorldLocation);
	WorldLocation += WorldDirection * ColliderToMouseProjDist;
	WorldLocation.Z = DraggableCollider->GetLocation().Z;

	if (DraggableActor->Drag(*DraggableCollider, WorldLocation))
	{
		FVector2D MouseLoc;
		ProjectWorldLocationToScreen(DraggableCollider->GetLocation(), MouseLoc);
		SetMouseLocation(MouseLoc.X, MouseLoc.Y);

		// HACK!
		// Sometimes (I can't find any deterministic way to replicate this error, but it can be a problem with the reading process of the procedural mesh normal array)
		// the table bound generation fails and it goes out of the map bound.
		// In order to fix this problem I regenerate the mesh and it's colliders (I could regenerate only the bound collider, but I want to be safer) when this error occurs.
		if (!DraggableActor->IsBetweenLocationBounds((FTACBoxCollider&)DraggableActor->GetBoundCollider()))
		{
			DraggableActor->Generate();
		}
	}
}

void ATACDefaultPlayerController::MoveHitActor() const
{
	if (!DraggableActor.IsValid())
	{
		return;
	}

	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	const FVector& PawnLocation = GetPawn()->GetActorLocation();
	const FVector& TableLocation = DraggableActor->GetCustomTransform().Location;

	const FVector NewTableLocation = GetTableMovedLocation(PawnLocation, WorldDirection, TableLocation);

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

	if (FVector::Distance(MovableTableSavedLocation, NewTableLocation) < 5.0f)
	{
		return;
	}

	const FVector TableMoveDir = NewTableLocation - MovableTableSavedLocation;
	if (DraggableActor->Move(TableMoveDir.GetSafeNormal() * TableMoveDir.Size()))
	{
		MovableTableSavedLocation = NewTableLocation;
	}
}
