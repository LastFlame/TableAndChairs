// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomShape.h"
#include "CustomSphereComponent.h"
#include "DrawDebugHelpers.h"
#include "CustomShapesRenderer.h"

ACustomShape::ACustomShape()
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
		SelectedSphereMaterial = (UMaterial*)SphereMaterial.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> TableMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Basic_Floor.M_Basic_Floor'"));
	if (SphereMaterial.Object != NULL)
	{
		SelectedTableMaterial = (UMaterial*)TableMaterial.Object;
	}
	
	RaycastColliders.Add(TopRightSphereComponent->GetCollider());
	RaycastColliders.Add(BottomRightSphereComponent->GetCollider());
	RaycastColliders.Add(TopLeftSphereComponent->GetCollider());
	RaycastColliders.Add(BottomLeftSphereComponent->GetCollider());

	
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

bool ACustomShape::Drag(const FCustomRaycastBaseCollider* Collider, const FVector& DragLocation)
{
	if (TableComponent->GetCustomShapeBuffer().VertexBuffer.Num() == 0)
	{
		return false;
	}

	FCustomSphereRaycastCollider* SphereCollider = (FCustomSphereRaycastCollider*)Collider;
	if (SphereCollider == nullptr)
	{
		return false;
	}

	if (FVector::Distance(Collider->GetLocation(), DragLocation) < DragThreshold)
	{
		return false;
	}

	FCustomCubeMeshData Table =
	{
		(FCustomCubeQuads*)(&TableComponent->GetCustomShapeBuffer().VertexBuffer[0]),
		(FCustomCubeQuads*)(&TableComponent->GetCustomShapeBuffer().NormalsBuffer[0])
	};

	FVector DragDir = (DragLocation - Collider->GetLocation()).GetSafeNormal();

	if (TopRightSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->BackQuad.TopRight, Table.Normals->RightQuad.TopRight, DragDir, DragThreshold, -DragThreshold))
		{
			Generate();
			return true;
		}
	}

	if (BottomRightSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->FrontQuad.TopRight, Table.Normals->RightQuad.TopRight, DragDir, -DragThreshold, -DragThreshold))
		{
			Generate();
			return true;
		}
	}

	if (TopLeftSphereComponent->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->BackQuad.TopRight, Table.Normals->LeftQuad.TopRight, DragDir, DragThreshold, DragThreshold))
		{
			Generate();
			return true;
		}
	}

	if (BottomLeftSphereComponent->GetCollider() == SphereCollider)
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

void ACustomShape::ResetSelection()
{
	UE_LOG(LogTemp, Warning, TEXT("CustomShape ResetSelection"));

	TableComponent->SetMaterial(0, nullptr);
	if (HitSphere != nullptr)
	{
		HitSphere->SetMaterial(0, nullptr);
	}
}

void ACustomShape::OnHit(const FCustomRaycastBaseCollider* Collider, const FVector& HitPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("CustomShape on Hit"));

	TableComponent->SetMaterial(0, SelectedTableMaterial);

	FCustomSphereRaycastCollider* SphereCollider = (FCustomSphereRaycastCollider*)Collider;
	if (SphereCollider == nullptr)
	{
		return;
	}

	if (HitSphere != nullptr)
	{
		HitSphere->SetMaterial(0, nullptr);
	}

	if (TopRightSphereComponent->GetCollider() == SphereCollider)
	{
		HitSphere = TopRightSphereComponent;
	}
	else if (BottomRightSphereComponent->GetCollider() == SphereCollider)
	{
		HitSphere = BottomRightSphereComponent;
	}
	else if (TopLeftSphereComponent->GetCollider() == SphereCollider)
	{
		HitSphere = TopLeftSphereComponent;
	}
	else if (BottomLeftSphereComponent->GetCollider() == SphereCollider)
	{
		HitSphere = BottomLeftSphereComponent;
	}

	if (HitSphere != nullptr)
	{
		HitSphere->SetMaterial(0, SelectedSphereMaterial);
	}
}

void ACustomShape::ShowDebugBoxCollider() const
{
	if (TableComponent == nullptr || TableComponent->GetCollider() == nullptr)
	{
		return;
	}

	const FVector& BackBottomLeftStart = TableComponent->GetCollider()->GetMinBounds();
	const FVector& FrontTopRightStart = TableComponent->GetCollider()->GetMaxBounds();

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
