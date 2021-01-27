// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomShape.h"
#include "TAC/CustomShapes/Components/CustomSphereComponent.h"
#include "DrawDebugHelpers.h"
#include "TAC/CustomShapes/CustomShapesRenderer.h"

ACustomShape::ACustomShape() : RaycastCollidersArray(*this)
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;
	
	TableComponent = CreateDefaultSubobject<UCustomTableComponent>(TEXT("TableComponent"));
	TableComponent->SetupAttachment(RootComponent);

	TopRightSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("TopRightSphereComponent"));
	TopRightSphereComponent->SetupAttachment(RootComponent);

	BottomRightSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("BottomRightSphereComponent"));
	BottomRightSphereComponent->SetupAttachment(RootComponent);

	TopLeftSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("TopLeftSphereComponent"));
	TopLeftSphereComponent->SetupAttachment(RootComponent);

	BottomLeftSphereComponent = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("BottomLeftSphereComponent"));
	BottomLeftSphereComponent->SetupAttachment(RootComponent);

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

	TableComponent->GetCollider().OnHit.AddUObject(this, &ACustomShape::OnBoundColliderHit);
	TableComponent->GetCollider().OnHitChanged.AddUObject(this, &ACustomShape::OnBoundColliderHitChanged);

	TopRightSphereComponent->GetCollider().OnHit.AddUObject(this, &ACustomShape::OnTopRightSphereHit);
	BottomRightSphereComponent->GetCollider().OnHit.AddUObject(this, &ACustomShape::OnBottomRightSphereHit);
	TopLeftSphereComponent->GetCollider().OnHit.AddUObject(this, &ACustomShape::OnTopLeftSphereHit);
	BottomLeftSphereComponent->GetCollider().OnHit.AddUObject(this, &ACustomShape::OnBottomLeftSphereHit);
}

void ACustomShape::Generate()
{
	FlushPersistentDebugLines(GetWorld());

	FCustomCubeMeshData Table = TableComponent->Draw();
	TableComponent->GenerateCollider();
	ShowDebugBoxCollider();

	TopRightSphereComponent->Draw(Table.Positions->TopQuad.TopRight, SphereRadius);
	TopRightSphereComponent->GenerateCollider();

	BottomRightSphereComponent->Draw(Table.Positions->TopQuad.BottomRight, SphereRadius);
	BottomRightSphereComponent->GenerateCollider();

	TopLeftSphereComponent->Draw(Table.Positions->TopQuad.TopLeft, SphereRadius);
	TopLeftSphereComponent->GenerateCollider();

	BottomLeftSphereComponent->Draw(Table.Positions->TopQuad.BottomLeft, SphereRadius);
	BottomLeftSphereComponent->GenerateCollider();
}

bool ACustomShape::Drag(const FCustomRaycastBaseCollider& Collider, const FVector& DragLocation)
{
	if (TableComponent->GetCustomShapeBuffer().VertexBuffer.Num() == 0)
	{
		return false;
	}

	FCustomSphereRaycastCollider* SphereCollider = (FCustomSphereRaycastCollider*)&Collider;
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
			TableComponent->GetTransform().Size.X = TableSizeX;
			TableComponent->GetTransform().Location.X -= ForwardDragDistance * (ForwardDotProd / ForwardDotProdAbs);
			bDragged = true;
		}
	}

	if (RightDotProdAbs > 0.5f)
	{
		float TableSizeY = TableComponent->GetTransform().Size.Y + abs(RightDragDistance) * (RightDotProd / RightDotProdAbs);

		if (TableSizeY >= TableComponent->GetTableMinSize().Y)
		{
			TableComponent->GetTransform().Size.Y = TableSizeY;
			TableComponent->GetTransform().Location.Y -= RightDragDistance * (RightDotProd / RightDotProdAbs);
			bDragged = true;
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

void ACustomShape::ShowDebugBoxCollider() const
{
	if (TableComponent == nullptr)
	{
		return;
	}

	const FVector& BackBottomLeftStart = TableComponent->GetCollider().GetMinBounds();
	const FVector& FrontTopRightStart = TableComponent->GetCollider().GetMaxBounds();

	const float XSize = FrontTopRightStart.X - BackBottomLeftStart.X;
	const float YSize = FrontTopRightStart.Y - BackBottomLeftStart.Y;
	const float ZSize = FrontTopRightStart.Z - BackBottomLeftStart.Z;

	const FVector BackTopLeftStart = BackBottomLeftStart + FVector::UpVector * ZSize;
	const FVector BackBottomRightStart = BackBottomLeftStart + FVector::RightVector * YSize;

	const FVector FrontBottomRightStart = FrontTopRightStart + FVector::DownVector * ZSize;
	const FVector FrontBottomLeftStart = FrontBottomRightStart + FVector::LeftVector * YSize;

	DrawDebugLine(GetWorld(), BackBottomLeftStart, BackBottomLeftStart + FVector::RightVector * YSize, FColor::Red, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), BackBottomLeftStart, BackBottomLeftStart + FVector::UpVector * ZSize, FColor::Red, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), BackBottomLeftStart, BackBottomLeftStart + FVector::ForwardVector * XSize, FColor::Red, true, -1.0f, 0, 1.0f);

	DrawDebugLine(GetWorld(), FrontTopRightStart, FrontTopRightStart + FVector::LeftVector * YSize, FColor::Red, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), FrontTopRightStart, FrontTopRightStart + FVector::DownVector * ZSize, FColor::Red, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), FrontTopRightStart, FrontTopRightStart + FVector::BackwardVector * XSize, FColor::Red, true, -1.0f, 0, 1.0f);

	DrawDebugLine(GetWorld(), FrontBottomRightStart, FrontBottomRightStart + FVector::BackwardVector * XSize, FColor::Red, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), FrontBottomRightStart, FrontBottomRightStart + FVector::LeftVector * YSize, FColor::Red, true, -1.0f, 0, 1.0f);

	DrawDebugLine(GetWorld(), BackTopLeftStart, BackTopLeftStart + FVector::ForwardVector * XSize, FColor::Red, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), BackTopLeftStart, BackTopLeftStart + FVector::RightVector * YSize, FColor::Red, true, -1.0f, 0, 1.0f);

	DrawDebugLine(GetWorld(), BackBottomRightStart, BackBottomRightStart + FVector::UpVector * ZSize, FColor::Red, true, -1.0f, 0, 1.0f);
	DrawDebugLine(GetWorld(), FrontBottomLeftStart, FrontBottomLeftStart + FVector::UpVector * ZSize, FColor::Red, true, -1.0f, 0, 1.0f);
}
