// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomShape.h"
#include "CustomSphereComponent.h"
#include "DrawDebugHelpers.h"
#include "CustomShapesRenderer.h"
#include "GenericPlatform/GenericPlatformMath.h"

ACustomShape::ACustomShape() : TableTransform({ 0.0f, 0.0f, 73.0f }, { 0.0f, 0.0f, 0.0f }, { 32.0f, 32.0f, 1.5f }), TableLegsSize(2.0f, 2.0f),
							   TableMinSize(TableTransform.Size.X, TableTransform.Size.Y)
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;
	
	TableAndChairsMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TableAndChairsProceduralMesh"));
	TableAndChairsMeshComponent->SetupAttachment(RootComponent);

	TopRightSphere = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("TopRightSphereComponent"));
	TopRightSphere->SetupAttachment(RootComponent);

	BottomRightSphere = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("BottomRightSphereComponent"));
	BottomRightSphere->SetupAttachment(RootComponent);

	TopLeftSphere = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("TopLeftSphereComponent"));
	TopLeftSphere->SetupAttachment(RootComponent);

	BottomLeftSphere = CreateDefaultSubobject<UCustomSphereComponent>(TEXT("BottomLeftSphereComponent"));
	BottomLeftSphere->SetupAttachment(RootComponent);

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
	
	RaycastColliders.Add(TopRightSphere->GetCollider());
	RaycastColliders.Add(BottomRightSphere->GetCollider());
	RaycastColliders.Add(TopLeftSphere->GetCollider());
	RaycastColliders.Add(BottomLeftSphere->GetCollider());

	BoundBoxCollider.SetHittableActor(this);

	// Preallocate some custom shape buffer memory.
	constexpr int32 ReserveSize = 2000;
	TableAndChairsCustomShapeBuffers.VertexBuffer.Reserve(ReserveSize);
	TableAndChairsCustomShapeBuffers.IndexBuffer.Reserve(ReserveSize * 6);
	TableAndChairsCustomShapeBuffers.NormalsBuffer.Reserve(ReserveSize);
	TableAndChairsCustomShapeBuffers.UV0Buffer.Reserve(ReserveSize);
	TableAndChairsCustomShapeBuffers.TangentsBuffer.Reserve(ReserveSize);
}

void ACustomShape::OnHit(const FCustomRaycastBaseCollider* Collider, const FVector& HitPoint)
{
	UE_LOG(LogTemp, Warning, TEXT("CustomShape on Hit"));

	TableAndChairsMeshComponent->SetMaterial(0, SelectedTableMaterial);

	FCustomSphereRaycastCollider* SphereCollider = (FCustomSphereRaycastCollider*)Collider;
	if (SphereCollider == nullptr)
	{
		return;
	}

	if (HitSphere != nullptr)
	{
		HitSphere->SetMaterial(0, nullptr);
	}

	if (TopRightSphere->GetCollider() == SphereCollider)
	{
		HitSphere = TopRightSphere;
	}
	else if (BottomRightSphere->GetCollider() == SphereCollider)
	{
		HitSphere = BottomRightSphere;
	}
	else if (TopLeftSphere->GetCollider() == SphereCollider)
	{
		HitSphere = TopLeftSphere;
	}
	else if (BottomLeftSphere->GetCollider() == SphereCollider)
	{
		HitSphere = BottomLeftSphere;
	}

	if (HitSphere != nullptr)
	{
		HitSphere->SetMaterial(0, SelectedSphereMaterial);
	}
}

bool ACustomShape::Drag(const FCustomRaycastBaseCollider* Collider, const FVector& DragLocation)
{
	if (TableAndChairsCustomShapeBuffers.VertexBuffer.Num() == 0)
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
		(FCustomCubeQuads*)(&TableAndChairsCustomShapeBuffers.VertexBuffer[0]),
		(FCustomCubeQuads*)(&TableAndChairsCustomShapeBuffers.NormalsBuffer[0])
	};

	FVector DragDir = (DragLocation - Collider->GetLocation()).GetSafeNormal();

	if (TopRightSphere->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->BackQuad.TopRight, Table.Normals->RightQuad.TopRight, DragDir, DragThreshold, -DragThreshold))
		{
			Generate();
			return true;
		}
	}

	if (BottomRightSphere->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->FrontQuad.TopRight, Table.Normals->RightQuad.TopRight , DragDir, -DragThreshold, -DragThreshold))
		{
			Generate();
			return true;
		}
	}

	if (TopLeftSphere->GetCollider() == SphereCollider)
	{
		if (DragEdge(Table.Normals->BackQuad.TopRight, Table.Normals->LeftQuad.TopRight, DragDir, DragThreshold, DragThreshold))
		{
			Generate();
			return true;
		}
	}

	if (BottomLeftSphere->GetCollider() == SphereCollider)
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
		float TableSizeX = TableTransform.Size.X + abs(ForwardDragDistance) * (ForwardDotProd / ForwardDotProdAbs);

		if (TableSizeX >= TableMinSize.X)
		{
			TableTransform.Size.X = TableSizeX;
			TableTransform.Location.X -= ForwardDragDistance * (ForwardDotProd / ForwardDotProdAbs);
			bDragged = true;
		}
	}

	if (RightDotProdAbs > 0.5f)
	{
		float TableSizeY = TableTransform.Size.Y + abs(RightDragDistance) * (RightDotProd / RightDotProdAbs);

		if (TableSizeY >= TableMinSize.Y)
		{
			TableTransform.Size.Y = TableSizeY;
			TableTransform.Location.Y -= RightDragDistance * (RightDotProd / RightDotProdAbs);
			bDragged = true;
		}
	}

	return bDragged;
}

void ACustomShape::Generate()
{
	TableTransform.Size.X = FGenericPlatformMath::Max(TableTransform.Size.X, TableMinSize.X);
	TableTransform.Size.Y = FGenericPlatformMath::Max(TableTransform.Size.Y, TableMinSize.Y);

	TableAndChairsCustomShapeBuffers.Reset();

	FCustomCubeMeshData Table;
	CustomShapesRenderer::BeginScene(TableAndChairsCustomShapeBuffers, *TableAndChairsMeshComponent);
	{
		Table = DrawTable(TableTransform, TableLegsSize);
		DrawChairs(Table);
	}
	CustomShapesRenderer::EndScene();

	TopRightSphere->Draw(Table.Positions->TopQuad.TopRight, SphereRadius);
	TopRightSphere->GenerateCollider();

	BottomRightSphere->Draw(Table.Positions->TopQuad.BottomRight, SphereRadius);
	BottomRightSphere->GenerateCollider();

	TopLeftSphere->Draw(Table.Positions->TopQuad.TopLeft, SphereRadius);
	TopLeftSphere->GenerateCollider();

	BottomLeftSphere->Draw(Table.Positions->TopQuad.BottomLeft, SphereRadius);
	BottomLeftSphere->GenerateCollider();

	const float DistanceFromMidToBackChair = TableTransform.Size.X + ChairDistanceFromTableSide + ChairSize.X + ChairBackRestSize.X;
	const FVector MinStartLocation = TableTransform.Location + Table.Normals->TopQuad.TopRight * 20.f;
	const FVector MaxStartLocation = TableTransform.Location - Table.Normals->TopQuad.TopRight * TableTransform.Location.Z; 

	BoundBoxCollider.SetMaxBounds((MinStartLocation + Table.Normals->FrontQuad.TopRight * DistanceFromMidToBackChair)
		+ Table.Normals->RightQuad.TopRight * DistanceFromMidToBackChair);

	BoundBoxCollider.SetMinBounds((MaxStartLocation + Table.Normals->BackQuad.TopRight * DistanceFromMidToBackChair)
		+ Table.Normals->LeftQuad.TopRight * DistanceFromMidToBackChair);
}

void ACustomShape::ResetSelection()
{
	UE_LOG(LogTemp, Warning, TEXT("CustomShape ResetSelection"));

	TableAndChairsMeshComponent->SetMaterial(0, nullptr);
	if (HitSphere != nullptr)
	{
		HitSphere->SetMaterial(0, nullptr);
	}
}

FCustomCubeMeshData ACustomShape::DrawTable(const FCustomCubeTransform& Transform, const FVector2D& LegsSize)
{
	FCustomCubeMeshData TableVerticesData = CustomShapesRenderer::DrawCube(Transform);
	DrawFrontLeftLeg(Transform, TableVerticesData, LegsSize);
	DrawFrontRightLeg(Transform, TableVerticesData, LegsSize);
	DrawBackLeftLeg(Transform, TableVerticesData, LegsSize);
	DrawBackRightLeg(Transform, TableVerticesData, LegsSize);

	return TableVerticesData;
}

void ACustomShape::DrawChairs(const FCustomCubeMeshData& TableCustomVertices)
{
	// FRONT CHAIRS
	const FCustomQuadVertices& TableFrontQuad = TableCustomVertices.Positions->FrontQuad;
	const FVector& TableFrontOffset = TableCustomVertices.Normals->FrontQuad.TopRight;
	DrawSequenceOfChairs(TableFrontQuad.BottomRight, TableFrontQuad.BottomLeft, TableFrontOffset, 180.0f);

	// LEFT CHARIS
	const FCustomQuadVertices& TableLeftQuad = TableCustomVertices.Positions->LeftQuad;
	const FVector& TableLeftOffset = TableCustomVertices.Normals->LeftQuad.TopRight;
	DrawSequenceOfChairs(TableLeftQuad.BottomRight, TableLeftQuad.BottomLeft, TableLeftOffset, 90.0f);

	// BACK CHAIRS
	const FCustomQuadVertices& TableBackQuad = TableCustomVertices.Positions->BackQuad;
	const FVector& TableBackOffset = TableCustomVertices.Normals->BackQuad.TopRight;
	DrawSequenceOfChairs(TableBackQuad.BottomRight, TableBackQuad.BottomLeft, TableBackOffset, 0.0f);

	// RIGHT CHARIS
	const FCustomQuadVertices& TableRightQuad = TableCustomVertices.Positions->RightQuad;
	const FVector& TableRightOffset = TableCustomVertices.Normals->RightQuad.TopRight;
	DrawSequenceOfChairs(TableRightQuad.BottomRight, TableRightQuad.BottomLeft, TableRightOffset, -90.0f);
}

void ACustomShape::DrawSequenceOfChairs(const FVector& TableRightCorner, const FVector& TableLeftCorner, const FVector& OffsetDirection, float Rotation)
{
	//	To draw a sequence of chairs we start from the right leg of the current side of the table plus a distance value.
	//	We take the end point of the side (aka the left leg of the current side of the table plus a distance value),
	//	then we calculate the distance between those points.
	//	Once we have the distance we interpolate it and we place a chair if possible.
	//	Every loop we increase the traveled distance by the chair size and the distance between each chair.
	//
	//	Start and End points offsets is useful to place each chairs away from the table, otherwise they can clip with the table.
	//	
	//	s = start chair
	//	c = center of the chair
	//	e = end chair
	//	
	//	Sequence direction
	//	------------------>
	//	 _______________________
	//	|						|
	//	|						|
	//	|---s--c--e---s--c--e---|
	//	|						|
	//

	
	// Direction to place every chairs next to each others.
	const FVector SequenceDirection = (TableLeftCorner - TableRightCorner).GetSafeNormal(); 

	const FVector OffsetFromTableLegs = SequenceDirection * (TableLegsSize.Y * 2 + DistanceBetweenChairs);

	// We assume that the table x axis rotation is always 0.
	const FVector OffSetFromTableBottom = FVector::DownVector * ChairDistanceFromTableBottom;

	const FVector OffSetFromTableSide = OffsetDirection * ChairDistanceFromTableSide;

	const FVector StartPosition = TableRightCorner + OffsetFromTableLegs + OffSetFromTableBottom + OffSetFromTableSide;
	const FVector EndPosition = TableLeftCorner + OffsetFromTableLegs * -1 + OffSetFromTableBottom + OffSetFromTableSide;

	const float DistanceToTravel = FVector::Dist(StartPosition, EndPosition);
	float DistanceTraveled = ChairSize.Y * 2;

	while (DistanceTraveled <= DistanceToTravel)
	{
		//DrawDebugSphere(GetWorld(), StartPosition + SequenceDirection * DistanceTraveled, 1.0f, 20, FColor::Black, true);
		//UE_LOG(LogTemp, Warning, TEXT("Dist traveled %f"), DistTraveled);
		//UE_LOG(LogTemp, Warning, TEXT("Dist traveled half %f"), DistTraveled - ChairSize.Y);

		FCustomCubeTransform ChairTransform
		{
			ChairTransform.Location = StartPosition + SequenceDirection * (DistanceTraveled - ChairSize.Y),
			ChairTransform.Rotation = { 0.0f, 0.0f, Rotation + TableTransform.Rotation.Z},
			ChairTransform.Size = ChairSize
		};

		//DrawDebugSphere(GetWorld(), ChairTransform.Location, 1.0f, 20, FColor::Purple, true);

		DrawChair(ChairTransform);

		DistanceTraveled += ChairSize.Y * 2 + DistanceBetweenChairs;
	}

	/*DrawDebugLine(GetWorld(), StartPosition, EndPosition, FColor::Red, true);
	DrawDebugSphere(GetWorld(), StartPosition, 1.0f, 20, FColor::Green, true);
	DrawDebugSphere(GetWorld(), EndPosition, 1.0f, 20, FColor::Yellow, true);*/
}

FCustomCubeMeshData ACustomShape::DrawChair(const FCustomCubeTransform& Transform)
{
	FCustomCubeMeshData ChairSeat = DrawTable(Transform, ChairLegsSize);

	const FVector& ChairForwardDir = ChairSeat.Normals->FrontQuad.TopLeft;
	const FVector& ChairLeftDir = ChairSeat.Normals->RightQuad.TopLeft;
	const FVector& ChairUpDir = ChairSeat.Normals->TopQuad.TopLeft;

	FCustomCubeTransform ChairBackRestTransform;
	ChairBackRestTransform.Location = ChairSeat.Positions->TopQuad.TopLeft;
	ChairBackRestTransform.Location += ChairForwardDir * ChairBackRestSize.X;
	ChairBackRestTransform.Location += ChairLeftDir * Transform.Size.Y;
	ChairBackRestTransform.Location += ChairUpDir * ChairBackRestSize.Z;

	ChairBackRestTransform.Rotation = Transform.Rotation;

	ChairBackRestTransform.Size.X = ChairBackRestSize.X;
	ChairBackRestTransform.Size.Y = Transform.Size.Y;
	ChairBackRestTransform.Size.Z = ChairBackRestSize.Z;

	CustomShapesRenderer::DrawCube(ChairBackRestTransform);

	return ChairSeat;
}

FCustomCubeMeshData ACustomShape::DrawFrontLeftLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize)
{
	const float Height = (Transform.Location.Z - Transform.Size.Z) * 0.5f;

	const FVector& ForwardDir = VerticesData.Normals->FrontQuad.TopLeft;
	const FVector& LeftDir = VerticesData.Normals->RightQuad.TopLeft;
	const FVector& UpDir = VerticesData.Normals->TopQuad.TopLeft;

	FCustomCubeTransform LegTransform;
	LegTransform.Location = VerticesData.Positions->BottomQuad.BottomRight;
	LegTransform.Location -= ForwardDir * LegsSize.X;
	LegTransform.Location += LeftDir * LegsSize.Y;
	LegTransform.Location -= UpDir * Height;

	LegTransform.Rotation = Transform.Rotation;

	LegTransform.Size.X = LegsSize.X;
	LegTransform.Size.Y = LegsSize.Y;
	LegTransform.Size.Z = Height;

	return CustomShapesRenderer::DrawCube(LegTransform);
}

FCustomCubeMeshData ACustomShape::DrawFrontRightLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize)
{
	const float Height = (Transform.Location.Z - Transform.Size.Z) * 0.5f;

	const FVector& ForwardDir = VerticesData.Normals->FrontQuad.TopLeft;
	const FVector& LeftDir = VerticesData.Normals->RightQuad.TopLeft;
	const FVector& UpDir = VerticesData.Normals->TopQuad.TopLeft;

	FCustomCubeTransform LegTransform;
	LegTransform.Location = VerticesData.Positions->BottomQuad.BottomLeft;
	LegTransform.Location -= ForwardDir * LegsSize.X;
	LegTransform.Location -= LeftDir * LegsSize.Y;
	LegTransform.Location -= UpDir * Height;

	LegTransform.Rotation = Transform.Rotation;

	LegTransform.Size.X = LegsSize.X;
	LegTransform.Size.Y = LegsSize.Y;
	LegTransform.Size.Z = Height;

	return CustomShapesRenderer::DrawCube(LegTransform);
}

FCustomCubeMeshData ACustomShape::DrawBackLeftLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize)
{
	const float Height = (Transform.Location.Z - Transform.Size.Z) * 0.5f;

	const FVector& ForwardDir = VerticesData.Normals->FrontQuad.TopLeft;
	const FVector& LeftDir = VerticesData.Normals->RightQuad.TopLeft;
	const FVector& UpDir = VerticesData.Normals->TopQuad.TopLeft;

	FCustomCubeTransform LegTransform;
	LegTransform.Location = VerticesData.Positions->BottomQuad.TopRight;
	LegTransform.Location += ForwardDir * LegsSize.X;
	LegTransform.Location += LeftDir * LegsSize.Y;
	LegTransform.Location -= UpDir * Height;

	LegTransform.Rotation = Transform.Rotation;

	LegTransform.Size.X = LegsSize.X;
	LegTransform.Size.Y = LegsSize.Y;
	LegTransform.Size.Z = Height;

	return CustomShapesRenderer::DrawCube(LegTransform);
}

FCustomCubeMeshData ACustomShape::DrawBackRightLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize)
{
	const float Height = (Transform.Location.Z - Transform.Size.Z) * 0.5f;

	const FVector& ForwardDir = VerticesData.Normals->FrontQuad.TopLeft;
	const FVector& LeftDir = VerticesData.Normals->RightQuad.TopLeft;
	const FVector& UpDir = VerticesData.Normals->TopQuad.TopLeft;

	FCustomCubeTransform LegTransform;
	LegTransform.Location = VerticesData.Positions->BottomQuad.TopLeft;
	LegTransform.Location += ForwardDir * LegsSize.X;
	LegTransform.Location -= LeftDir * LegsSize.Y;
	LegTransform.Location -= UpDir * Height;

	LegTransform.Rotation = Transform.Rotation;

	LegTransform.Size.X = LegsSize.X;
	LegTransform.Size.Y = LegsSize.Y;
	LegTransform.Size.Z = Height;

	return CustomShapesRenderer::DrawCube(LegTransform);
}