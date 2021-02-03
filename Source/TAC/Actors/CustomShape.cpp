// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomShape.h"
#include "TAC/CustomShapes/Components/CustomSphereComponent.h"
#include "DrawDebugHelpers.h"
#include "TAC/CustomShapes/CustomShapesRenderer.h"
#include "TAC/CustomShapeTemplateDataAsset.h"
#include "TAC/CustomCollisions/CustomCollisionSystem.h"

ACustomShape::ACustomShape() : RaycastCollidersArray(*this)
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;
	
	TableComponent = CreateDefaultSubobject<UCustomTableComponent>(TEXT("TableComponent"));
	TableComponent->SetupAttachment(RootComponent);
	TableComponent->GetCollider().SetFlag(ECustomCollisionFlags::Dynamic);

	TopRightSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("TopRightSphereComponent"));
	TopRightSphereComponent->SetupAttachment(RootComponent);
	TopRightSphereComponent->GetCollider().SetFlag(ECustomCollisionFlags::Dynamic);

	BottomRightSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("BottomRightSphereComponent"));
	BottomRightSphereComponent->SetupAttachment(RootComponent);
	BottomRightSphereComponent->GetCollider().SetFlag(ECustomCollisionFlags::Dynamic);

	TopLeftSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("TopLeftSphereComponent"));
	TopLeftSphereComponent->SetupAttachment(RootComponent);
	TopLeftSphereComponent->GetCollider().SetFlag(ECustomCollisionFlags::Dynamic);

	BottomLeftSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("BottomLeftSphereComponent"));
	BottomLeftSphereComponent->SetupAttachment(RootComponent);
	BottomLeftSphereComponent->GetCollider().SetFlag(ECustomCollisionFlags::Dynamic);

	static ConstructorHelpers::FObjectFinder<UCustomShapeTemplateDataAsset> CustomShapeDataAsset(TEXT("CustomShapeTemplateDataAsset'/Game/TAC/CustomShapeTemplateDataAsset.CustomShapeTemplateDataAsset'"));
	if (CustomShapeDataAsset.Object != NULL)
	{
		CustomShapeTemplateData = CustomShapeDataAsset.Object;
		SphereRadius = CustomShapeTemplateData->GetSphereRadius();
		DragThreshold = CustomShapeTemplateData->GetDragThreashold();
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> SphereMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold'"));
	if (SphereMaterial.Object != NULL)
	{
		OnSphereSelectedMat = (UMaterial*)SphereMaterial.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> TableMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Basic_Floor.M_Basic_Floor'"));
	if (SphereMaterial.Object != NULL)
	{
		OnSelectedTableMat = (UMaterial*)TableMaterial.Object;
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
	ACustomShape* CustomShape = World->SpawnActor<ACustomShape>(FVector::ZeroVector, FRotator::ZeroRotator);
	CustomShape->SetCustomLocation(Location.X, Location.Y);
	CustomShape->Generate();
}

void ACustomShape::Generate()
{
	FlushPersistentDebugLines(GetWorld());

	FCustomCubeMeshData Table = TableComponent->Draw();
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

bool ACustomShape::Drag(const FCustomBaseCollider& Collider, const FVector& DragLocation)
{
	if (TableComponent->GetCustomShapeBuffer().VertexBuffer.Num() == 0)
	{
		return false;
	}

	FCustomSphereCollider* SphereCollider = (FCustomSphereCollider*)&Collider;
	if (SphereCollider == nullptr)
	{
		return false;
	}

	if (FVector::Distance(Collider.GetLocation(), DragLocation) < DragThreshold)
	{
		return false;
	}

	FCustomCubeMeshData Table =
	{
		(FCustomCubeQuads*)(&TableComponent->GetCustomShapeBuffer().VertexBuffer[0]),
		(FCustomCubeQuads*)(&TableComponent->GetCustomShapeBuffer().NormalsBuffer[0])
	};

	FVector DragDir = (DragLocation - Collider.GetLocation()).GetSafeNormal();

	if (&TopRightSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->BackQuad.TopRight, Table.Normals->RightQuad.TopRight, DragDir, DragThreshold, -DragThreshold))
		{
			Generate();
			return true;
		}
	}

	if (&BottomRightSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->FrontQuad.TopRight, Table.Normals->RightQuad.TopRight, DragDir, -DragThreshold, -DragThreshold))
		{
			Generate();
			return true;
		}
	}

	if (&TopLeftSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->BackQuad.TopRight, Table.Normals->LeftQuad.TopRight, DragDir, DragThreshold, DragThreshold))
		{
			Generate();
			return true;
		}
	}

	if (&BottomLeftSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->FrontQuad.TopRight, Table.Normals->LeftQuad.TopRight, DragDir, -DragThreshold, DragThreshold))
		{
			Generate();
			return true;
		}
	}

	return false;
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
			FCustomCubeTransform TableTransform = TableComponent->GetTransform();
			TableTransform.Size.X = TableSizeX;
			TableTransform.Location.X -= ForwardDragDistance * (ForwardDotProd / ForwardDotProdAbs);

			TableComponent->GetCollider().SetFlag(ECustomCollisionFlags::Static);

			FCustomBoxCollider BoxCollider;
			TableComponent->CreateCollider(TableTransform, BoxCollider);

			//FlushPersistentDebugLines(GetWorld());
			//ShowDebugBoxCollider(BoxCollider, FColor::Green);

			CustomCollisionSystem::FCustomCollisionResult CollisionResult;
			if (!CustomCollisionSystem::BoxTrace(BoxCollider, ECustomCollisionFlags::Static, CollisionResult))
			{
				TableComponent->GetTransform() = TableTransform;
				bDragged = true;
			}

			TableComponent->GetCollider().SetFlag(ECustomCollisionFlags::Dynamic);
		}
	}

	if (RightDotProdAbs > 0.5f)
	{
		float TableSizeY = TableComponent->GetTransform().Size.Y + abs(RightDragDistance) * (RightDotProd / RightDotProdAbs);

		if (TableSizeY >= TableComponent->GetTableMinSize().Y)
		{
			FCustomCubeTransform TableTransform = TableComponent->GetTransform();
			TableTransform.Size.Y = TableSizeY;
			TableTransform.Location.Y -= RightDragDistance * (RightDotProd / RightDotProdAbs);

			TableComponent->GetCollider().SetFlag(ECustomCollisionFlags::Static);

			FCustomBoxCollider BoxCollider;
			TableComponent->CreateCollider(TableTransform, BoxCollider);
			//FlushPersistentDebugLines(GetWorld());
			//ShowDebugBoxCollider(BoxCollider, FColor::Green);

			CustomCollisionSystem::FCustomCollisionResult CollisionResult;
			if (!CustomCollisionSystem::BoxTrace(BoxCollider, ECustomCollisionFlags::Static, CollisionResult))
			{
				TableComponent->GetTransform() = TableTransform;
				bDragged = true;
			}

			TableComponent->GetCollider().SetFlag(ECustomCollisionFlags::Dynamic);
		}
	}

	return bDragged;
}

void ACustomShape::OnBoundColliderHit(const FVector& HitPoint)
{
	TableComponent->SetMaterial(0, OnSelectedTableMat);
}

void ACustomShape::OnBoundColliderHitChanged()
{
	if (PrevHitSphere != nullptr)
	{
		PrevHitSphere->SetMaterial(0, nullptr);
	}

	TableComponent->SetMaterial(0, nullptr);
}

void ACustomShape::OnTopRightSphereHit(const FVector& HitPoint)
{
	if (PrevHitSphere)
	{
		PrevHitSphere->SetMaterial(0, nullptr);
	}

	TopRightSphereComponent->SetMaterial(0, OnSphereSelectedMat);
	PrevHitSphere = TopRightSphereComponent;
}

void ACustomShape::OnBottomRightSphereHit(const FVector& HitPoint)
{
	if (PrevHitSphere)
	{
		PrevHitSphere->SetMaterial(0, nullptr);
	}

	BottomRightSphereComponent->SetMaterial(0, OnSphereSelectedMat);
	PrevHitSphere = BottomRightSphereComponent;
}

void ACustomShape::OnTopLeftSphereHit(const FVector& HitPoint)
{
	if (PrevHitSphere)
	{
		PrevHitSphere->SetMaterial(0, nullptr);
	}

	TopLeftSphereComponent->SetMaterial(0, OnSphereSelectedMat);
	PrevHitSphere = TopLeftSphereComponent;
}

void ACustomShape::OnBottomLeftSphereHit(const FVector& HitPoint)
{
	if (PrevHitSphere)
	{
		PrevHitSphere->SetMaterial(0, nullptr);
	}

	BottomLeftSphereComponent->SetMaterial(0, OnSphereSelectedMat);
	PrevHitSphere = BottomLeftSphereComponent;
}

void ACustomShape::ShowDebugBoxCollider(const FCustomBoxCollider& BoxCollider, const FColor& Color) const
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
