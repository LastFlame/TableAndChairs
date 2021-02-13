// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomShape.h"
#include "TAC/CustomShapes/Components/CustomSphereComponent.h"
#include "DrawDebugHelpers.h"
#include "TAC/CustomShapeTemplateDataAsset.h"
#include "TACCollisionSystemModule/Public//TACCollisionSystem.h"
#include "TAC/CustomGameMode.h"

static TWeakObjectPtr<UTACCollisionSystem> CustomCollisionSystem;

static bool IsLocationBetweenBounds(const FVector2D& LocationBounds, const FVector& MinLocation, const FVector& MaxLocation)
{
	return !(MinLocation.Y < -LocationBounds.Y || MinLocation.X < -LocationBounds.X
		|| MaxLocation.Y > LocationBounds.Y || MaxLocation.X > LocationBounds.X);
}

ACustomShape::ACustomShape() : RaycastCollidersArray(*this)
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;
	
	TableComponent = CreateDefaultSubobject<UCustomTableComponent>(TEXT("TableComponent"));
	TableComponent->SetupAttachment(RootComponent);
	TableComponent->GetCollider().SetFlag(ETACCollisionFlags::Dynamic);

	TopRightSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("TopRightSphereComponent"));
	TopRightSphereComponent->SetupAttachment(RootComponent);
	TopRightSphereComponent->GetCollider().SetFlag(ETACCollisionFlags::Dynamic);

	BottomRightSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("BottomRightSphereComponent"));
	BottomRightSphereComponent->SetupAttachment(RootComponent);
	BottomRightSphereComponent->GetCollider().SetFlag(ETACCollisionFlags::Dynamic);

	TopLeftSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("TopLeftSphereComponent"));
	TopLeftSphereComponent->SetupAttachment(RootComponent);
	TopLeftSphereComponent->GetCollider().SetFlag(ETACCollisionFlags::Dynamic);

	BottomLeftSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("BottomLeftSphereComponent"));
	BottomLeftSphereComponent->SetupAttachment(RootComponent);
	BottomLeftSphereComponent->GetCollider().SetFlag(ETACCollisionFlags::Dynamic);

	static ConstructorHelpers::FObjectFinder<UCustomShapeTemplateDataAsset> CustomShapeDataAsset(TEXT("CustomShapeTemplateDataAsset'/Game/TAC/CustomShapeTemplateDataAsset.CustomShapeTemplateDataAsset'"));
	if (CustomShapeDataAsset.Object != nullptr)
	{
		CustomShapeTemplateData = CustomShapeDataAsset.Object;
		SphereRadius = CustomShapeTemplateData->GetSphereRadius();
		DragThreshold = CustomShapeTemplateData->GetDragThreashold();
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> SphereMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold'"));
	if (SphereMaterial.Object != nullptr)
	{
		OnSphereSelectedMat = (UMaterial*)SphereMaterial.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> SelectedTableMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Basic_Floor.M_Basic_Floor'"));
	if (SelectedTableMaterial.Object != nullptr)
	{
		OnSelectedTableMat = (UMaterial*)SelectedTableMaterial.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> MovingTableMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Basic_Wall.M_Basic_Wall'"));
	if (MovingTableMaterial.Object != nullptr)
	{
		OnMovingTableMat = (UMaterial*)MovingTableMaterial.Object;
	}

	RaycastCollidersArray.Add(TopRightSphereComponent->GetCollider());
	RaycastCollidersArray.Add(BottomRightSphereComponent->GetCollider());
	RaycastCollidersArray.Add(TopLeftSphereComponent->GetCollider());
	RaycastCollidersArray.Add(BottomLeftSphereComponent->GetCollider());

	TableComponent->GetCollider().OnLineTraceHit.AddUObject(this, &ACustomShape::OnBoundColliderHit);
	TableComponent->GetCollider().OnLineTraceHitChanged.AddUObject(this, &ACustomShape::OnBoundColliderHitChanged);

	TopRightSphereComponent->GetCollider().OnLineTraceHit.AddUObject(this, &ACustomShape::OnTopRightSphereHit);
	BottomRightSphereComponent->GetCollider().OnLineTraceHit.AddUObject(this, &ACustomShape::OnBottomRightSphereHit);
	TopLeftSphereComponent->GetCollider().OnLineTraceHit.AddUObject(this, &ACustomShape::OnTopLeftSphereHit);
	BottomLeftSphereComponent->GetCollider().OnLineTraceHit.AddUObject(this, &ACustomShape::OnBottomLeftSphereHit);
}


void ACustomShape::Create(UWorld* World, const FVector& Location)
{
	const ACustomGameMode* CustomGameMode = Cast<const ACustomGameMode>(World->GetAuthGameMode());
	if (CustomGameMode == nullptr)
	{
		return;
	}

	const UCustomShapeTemplateDataAsset& CustomShapeTemplateData = CustomGameMode->GetCustomShapeTemplateData();
	const FTACBoxCollider& TableDefaultBounds = CustomShapeTemplateData.GetDefaultTableBoundCollider();

	FTACBoxCollider TableToSpawnCollider;
	TableToSpawnCollider.SetMinBounds(Location + TableDefaultBounds.GetMinBounds());
	TableToSpawnCollider.SetMaxBounds(Location + TableDefaultBounds.GetMaxBounds());

	if (!IsLocationBetweenBounds(CustomShapeTemplateData.GetLocationBounds(), TableToSpawnCollider.GetMinBounds(), TableToSpawnCollider.GetMaxBounds()))
	{
		return;
	}

	FTACCollisionResult Result;
	if (World->GetSubsystem<UTACCollisionSystem>()->BoxTrace(TableToSpawnCollider, ETACCollisionFlags::Static, Result))
	{
		return;
	}

	ACustomShape* CustomShape = World->SpawnActor<ACustomShape>(FVector::ZeroVector, FRotator::ZeroRotator);
	CustomShape->SetCustomLocation(Location.X, Location.Y);
	CustomShape->Generate();
}

void ACustomShape::Generate()
{
	FlushPersistentDebugLines(GetWorld());

	FTACCubeMeshData Table = TableComponent->Draw();
	TableComponent->GenerateCollider();
	ShowDebugBoxCollider(TableComponent->GetCollider(), FColor::Red);

	TopRightSphereComponent->Draw(Table.Positions->TopQuad.TopRight, SphereRadius);
	TopRightSphereComponent->GenerateCollider();

	BottomRightSphereComponent->Draw(Table.Positions->TopQuad.BottomRight, SphereRadius);
	BottomRightSphereComponent->GenerateCollider();

	TopLeftSphereComponent->Draw(Table.Positions->TopQuad.TopLeft, SphereRadius);
	TopLeftSphereComponent->GenerateCollider();

	BottomLeftSphereComponent->Draw(Table.Positions->TopQuad.BottomLeft, SphereRadius);
	BottomLeftSphereComponent->GenerateCollider();
}

bool ACustomShape::Drag(const FTACBaseCollider& Collider, const FVector& DragLocation)
{
	if (TableComponent->GetCustomShapeBuffer().VertexBuffer.Num() == 0)
	{
		return false;
	}

	FTACSphereCollider* SphereCollider = (FTACSphereCollider*)&Collider;
	if (SphereCollider == nullptr)
	{
		return false;
	}

	if (FVector::Distance(Collider.GetLocation(), DragLocation) < DragThreshold)
	{
		return false;
	}

	FTACCubeMeshData Table =
	{
		(FTACCubeQuads*)(&TableComponent->GetCustomShapeBuffer().VertexBuffer[0]),
		(FTACCubeQuads*)(&TableComponent->GetCustomShapeBuffer().NormalsBuffer[0])
	};

	FVector DragDir = (DragLocation - Collider.GetLocation()).GetSafeNormal();

	if (&TopRightSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->BackQuad.TopRight, Table.Normals->RightQuad.TopRight, DragDir, DragThreshold, -DragThreshold))
		{
			Generate();
		}
	}

	if (&BottomRightSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->FrontQuad.TopRight, Table.Normals->RightQuad.TopRight, DragDir, -DragThreshold, -DragThreshold))
		{
			Generate();
		}
	}

	if (&TopLeftSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->BackQuad.TopRight, Table.Normals->LeftQuad.TopRight, DragDir, DragThreshold, DragThreshold))
		{
			Generate();
		}
	}

	if (&BottomLeftSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->FrontQuad.TopRight, Table.Normals->LeftQuad.TopRight, DragDir, -DragThreshold, DragThreshold))
		{
			Generate();
		}
	}

	return true;
}

bool ACustomShape::DragEdge(const FVector& ForwardDir, const FVector& RightDir, const FVector& DragDir, float ForwardDragDistance, float RightDragDistance)
{
	const float ForwardDotProd = FVector::DotProduct(ForwardDir, DragDir);
	const float RightDotProd = FVector::DotProduct(RightDir, DragDir);

	const float ForwardDotProdAbs = abs(ForwardDotProd);
	const float RightDotProdAbs = abs(RightDotProd);

	bool bDragged = false;

	if (ForwardDotProdAbs > 0.5f)
	{
		float TableSizeX = TableComponent->GetTransform().Size.X + abs(ForwardDragDistance) * (ForwardDotProd / ForwardDotProdAbs);

		if (TableSizeX >= TableComponent->GetTableMinSize().X)
		{
			FTACCubeTransform TableTransform = TableComponent->GetTransform();
			TableTransform.Size.X = TableSizeX;
			TableTransform.Location.X -= ForwardDragDistance * (ForwardDotProd / ForwardDotProdAbs);

			TableComponent->GetCollider().SetFlag(ETACCollisionFlags::Static);

			FTACBoxCollider BoxCollider;
			TableComponent->CreateCollider(TableTransform, BoxCollider);

			//FlushPersistentDebugLines(GetWorld());
			//ShowDebugBoxCollider(BoxCollider, FColor::Green);

			FTACCollisionResult CollisionResult;
			if (!CustomCollisionSystem->BoxTrace(BoxCollider, ETACCollisionFlags::Static, CollisionResult) && IsBetweenLocationBounds(BoxCollider))
			{
				TableComponent->GetTransform() = TableTransform;
				bDragged = true;
			}

			TableComponent->GetCollider().SetFlag(ETACCollisionFlags::Dynamic);
		}
	}

	if (RightDotProdAbs > 0.5f)
	{
		float TableSizeY = TableComponent->GetTransform().Size.Y + abs(RightDragDistance) * (RightDotProd / RightDotProdAbs);

		if (TableSizeY >= TableComponent->GetTableMinSize().Y)
		{
			FTACCubeTransform TableTransform = TableComponent->GetTransform();
			TableTransform.Size.Y = TableSizeY;
			TableTransform.Location.Y -= RightDragDistance * (RightDotProd / RightDotProdAbs);

			TableComponent->GetCollider().SetFlag(ETACCollisionFlags::Static);

			FTACBoxCollider BoxCollider;
			TableComponent->CreateCollider(TableTransform, BoxCollider);
			//FlushPersistentDebugLines(GetWorld());
			//ShowDebugBoxCollider(BoxCollider, FColor::Green);

			FTACCollisionResult CollisionResult;
			if (!CustomCollisionSystem->BoxTrace(BoxCollider, ETACCollisionFlags::Static, CollisionResult) && IsBetweenLocationBounds(BoxCollider))
			{
				TableComponent->GetTransform() = TableTransform;
				bDragged = true;
			}

			TableComponent->GetCollider().SetFlag(ETACCollisionFlags::Dynamic);
		}
	}

	return bDragged;
}

bool ACustomShape::Move(const FVector& Location)
{
	FTACBoxCollider BoxCollider;

	const FVector NewTableLocation = TableComponent->GetTransform().Location + Location;
	BoxCollider.SetLocation(NewTableLocation);
	
	FTACBoxCollider& TableCollider = TableComponent->GetCollider();
	const FVector NewMinBounds = TableCollider.GetMinBounds() + Location;
	const FVector NewMaxBounds = TableCollider.GetMaxBounds() + Location;

	BoxCollider.SetMinBounds(NewMinBounds);
	BoxCollider.SetMaxBounds(NewMaxBounds);

	BoxCollider.SetFlag(ETACCollisionFlags::Static);
	TableCollider.SetFlag(ETACCollisionFlags::Static);

	FTACCollisionResult CollisionResult;
	bool bCollisionSuccessul = CustomCollisionSystem->BoxTrace(BoxCollider, ETACCollisionFlags::Static, CollisionResult);

	TableCollider.SetFlag(ETACCollisionFlags::Dynamic);

	if(bCollisionSuccessul)
	{
		return false;
	}

	if (!IsBetweenLocationBounds(BoxCollider))
	{
		return false;
	}
	
	TableComponent->GetTransform().Location = NewTableLocation;
	Generate();
	return true;
}

void ACustomShape::ResetDraggableSphere()
{
	if (PrevHitSphere != nullptr)
	{
		PrevHitSphere->SetMaterial(0, nullptr);
	}
}

void ACustomShape::BeginPlay()
{
	Super::BeginPlay();
	CustomCollisionSystem = GetWorld()->GetSubsystem<UTACCollisionSystem>();
}

void ACustomShape::OnBoundColliderHit(const FVector& HitPoint)
{
	TableComponent->SetMaterial(0, OnSelectedTableMat);
}

void ACustomShape::OnBoundColliderHitChanged()
{
	ResetDraggableSphere();
	TableComponent->SetMaterial(0, nullptr);
}

void ACustomShape::OnTopRightSphereHit(const FVector& HitPoint)
{
	ResetDraggableSphere();

	TopRightSphereComponent->SetMaterial(0, OnSphereSelectedMat);
	PrevHitSphere = TopRightSphereComponent;
}

void ACustomShape::OnBottomRightSphereHit(const FVector& HitPoint)
{
	ResetDraggableSphere();

	BottomRightSphereComponent->SetMaterial(0, OnSphereSelectedMat);
	PrevHitSphere = BottomRightSphereComponent;
}

void ACustomShape::OnTopLeftSphereHit(const FVector& HitPoint)
{
	ResetDraggableSphere();

	TopLeftSphereComponent->SetMaterial(0, OnSphereSelectedMat);
	PrevHitSphere = TopLeftSphereComponent;
}

void ACustomShape::OnBottomLeftSphereHit(const FVector& HitPoint)
{
	ResetDraggableSphere();

	BottomLeftSphereComponent->SetMaterial(0, OnSphereSelectedMat);
	PrevHitSphere = BottomLeftSphereComponent;
}

bool ACustomShape::IsBetweenLocationBounds(const FTACBoxCollider& BoxCollider) const
{
	return IsLocationBetweenBounds(CustomShapeTemplateData->GetLocationBounds(), BoxCollider.GetMinBounds(), BoxCollider.GetMaxBounds());
}

void ACustomShape::ShowDebugBoxCollider(const FTACBoxCollider& BoxCollider, const FColor& Color) const
{
	if (TableComponent == nullptr)
	{
		return;
	}

	const FVector& BackBottomLeftStart = BoxCollider.GetMinBounds();
	const FVector& FrontTopRightStart = BoxCollider.GetMaxBounds();

	const float XSize = FrontTopRightStart.X - BackBottomLeftStart.X;
	const float YSize = FrontTopRightStart.Y - BackBottomLeftStart.Y;
	const float ZSize = FrontTopRightStart.Z - BackBottomLeftStart.Z;

	const FVector BackTopLeftStart = BackBottomLeftStart + FVector::UpVector * ZSize;
	const FVector BackBottomRightStart = BackBottomLeftStart + FVector::RightVector * YSize;

	const FVector FrontBottomRightStart = FrontTopRightStart + FVector::DownVector * ZSize;
	const FVector FrontBottomLeftStart = FrontBottomRightStart + FVector::LeftVector * YSize;

	DrawDebugLine(GetWorld(), BackBottomLeftStart, BackBottomLeftStart + FVector::RightVector * YSize, Color, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), BackBottomLeftStart, BackBottomLeftStart + FVector::UpVector * ZSize, Color, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), BackBottomLeftStart, BackBottomLeftStart + FVector::ForwardVector * XSize, Color, true, -1.0f, 0, 1.0f);

	DrawDebugLine(GetWorld(), FrontTopRightStart, FrontTopRightStart + FVector::LeftVector * YSize, Color, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), FrontTopRightStart, FrontTopRightStart + FVector::DownVector * ZSize, Color, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), FrontTopRightStart, FrontTopRightStart + FVector::BackwardVector * XSize, Color, true, -1.0f, 0, 1.0f);

	DrawDebugLine(GetWorld(), FrontBottomRightStart, FrontBottomRightStart + FVector::BackwardVector * XSize, Color, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), FrontBottomRightStart, FrontBottomRightStart + FVector::LeftVector * YSize, Color, true, -1.0f, 0, 1.0f);

	DrawDebugLine(GetWorld(), BackTopLeftStart, BackTopLeftStart + FVector::ForwardVector * XSize, Color, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), BackTopLeftStart, BackTopLeftStart + FVector::RightVector * YSize, Color, true, -1.0f, 0, 1.0f);

	DrawDebugLine(GetWorld(), BackBottomRightStart, BackBottomRightStart + FVector::UpVector * ZSize, Color, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), FrontBottomLeftStart, FrontBottomLeftStart + FVector::UpVector * ZSize, Color, true, -1.0f, 0, 1.0f);
}
