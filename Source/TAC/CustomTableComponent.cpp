// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomTableComponent.h"
#include "CustomShapesRenderer.h"
#include "GenericPlatform/GenericPlatformMath.h"

UCustomTableComponent::UCustomTableComponent(const FObjectInitializer& ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer),
	CustomTransform({ 0.0f, 0.0f, 73.0f }, { 0.0f, 0.0f, 0.0f }, { 32.0f, 32.0f, 1.5f }), TableLegsSize(2.0f, 2.0f), 
	TableMinSize(CustomTransform.Size.X, CustomTransform.Size.Y), DistanceBetweenChairs(10.0f), ChairDistanceFromTableSide(30.0f), 
	ChairDistanceFromTableBottom(30.0f), ChairSize(20.0f, 18.0f, 1.0f), ChairLegsSize(1.0f, 1.0f), ChairBackRestSize(1.0f, 0.0f, 18.0f)
{
	// Preallocate some custom shape buffer memory.
	constexpr int32 ReserveSize = 2000;
	CustomShapeBuffers.VertexBuffer.Reserve(ReserveSize);
	CustomShapeBuffers.IndexBuffer.Reserve(ReserveSize * 6);
	CustomShapeBuffers.NormalsBuffer.Reserve(ReserveSize);
	CustomShapeBuffers.UV0Buffer.Reserve(ReserveSize);
	CustomShapeBuffers.TangentsBuffer.Reserve(ReserveSize);
}

void UCustomTableComponent::DebugDraw()
{
	Draw();
}

FCustomCubeMeshData UCustomTableComponent::Draw()
{
	// Prevents the table size from becoming negative and containing less than 1 chair.
	CustomTransform.Size.X = FGenericPlatformMath::Max(CustomTransform.Size.X, TableMinSize.X);
	CustomTransform.Size.Y = FGenericPlatformMath::Max(CustomTransform.Size.Y, TableMinSize.Y);

	CustomShapeBuffers.Reset();
	LastDrawnTable.Normals = nullptr;

	CustomShapesRenderer::BeginScene(CustomShapeBuffers, *this);
	{
		LastDrawnTable = DrawTable(CustomTransform, TableLegsSize);
		DrawChairs(LastDrawnTable);
	}
	CustomShapesRenderer::EndScene();

	return LastDrawnTable;
}

FCustomCubeMeshData UCustomTableComponent::Draw(const FVector& Location)
{
	CustomTransform.Location = Location;
	return Draw();
}

void UCustomTableComponent::GenerateCollider()
{
	FCustomCubeQuads* TableNormals = nullptr;

	if (LastDrawnTable.Normals != nullptr)
	{
		TableNormals = LastDrawnTable.Normals;
	}
	else if (CustomShapeBuffers.NormalsBuffer.Num() != 0) // Fallback in case the mesh as been drawn but LastDrawnTable is null.
	{
		TableNormals = (FCustomCubeQuads*)(&CustomShapeBuffers.NormalsBuffer[0]); // Mimic the return value of CustomShapesRenderer::DrawCube().
	}

	if (TableNormals == nullptr)
	{
		return;
	}
	
	const FVector& TableTopNormal = TableNormals->TopQuad.TopRight;
	const FVector& TableFrontNormal = TableNormals->FrontQuad.TopRight;
	const FVector& TableRightNormal = TableNormals->RightQuad.TopRight;
	const FVector& TableLeftNormal = TableNormals->LeftQuad.TopRight;
	const FVector& TableBackNormal = TableNormals->BackQuad.TopRight;

	const float DistanceFromMidToFrontBackRestChair = CustomTransform.Size.X + ChairDistanceFromTableSide + ChairSize.X + ChairBackRestSize.X;
	const float DistanceFromtMidToRightBackRestChair = CustomTransform.Size.Y + ChairDistanceFromTableSide + ChairSize.Y + ChairBackRestSize.Y;
	const FVector MinStartLocation = CustomTransform.Location - TableTopNormal * (CustomTransform.Location.Z) ; 
	const FVector MaxStartLocation = CustomTransform.Location + TableTopNormal * 20.f;

	// Location: top left looking at the table after the chairs.
	CustomBoxCollider.SetMaxBounds((MaxStartLocation + TableFrontNormal * DistanceFromMidToFrontBackRestChair)
		+ TableRightNormal * DistanceFromtMidToRightBackRestChair);

	// Location: bottom right looking at the table after the chairs.
	CustomBoxCollider.SetMinBounds((MinStartLocation + TableBackNormal * DistanceFromMidToFrontBackRestChair)
		+ TableLeftNormal * DistanceFromtMidToRightBackRestChair);
}

void UCustomTableComponent::GenerateCollider(const FVector& MinBounds, const FVector& MaxBounds)
{
	CustomBoxCollider.SetMinBounds(MinBounds);
	CustomBoxCollider.SetMaxBounds(MaxBounds);
}

void UCustomTableComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ICustomRaycastHittable* HittableOwner = Cast<ICustomRaycastHittable>(GetOwner());
	if (HittableOwner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s owner must implment ICustomRaycastHittable to receive collision events"), *GetName());
	}

	CustomBoxCollider.SetHittableActor(HittableOwner);
}

FCustomCubeMeshData UCustomTableComponent::DrawTable(const FCustomCubeTransform& Transform, const FVector2D& LegsSize)
{
	FCustomCubeMeshData TableVerticesData = CustomShapesRenderer::DrawCube(Transform);
	DrawFrontLeftLeg(Transform, TableVerticesData, LegsSize);
	DrawFrontRightLeg(Transform, TableVerticesData, LegsSize);
	DrawBackLeftLeg(Transform, TableVerticesData, LegsSize);
	DrawBackRightLeg(Transform, TableVerticesData, LegsSize);

	return TableVerticesData;
}

void UCustomTableComponent::DrawChairs(const FCustomCubeMeshData& TableCustomVertices)
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

void UCustomTableComponent::DrawSequenceOfChairs(const FVector& TableRightCorner, const FVector& TableLeftCorner, const FVector& OffsetDirection, float Rotation)
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
		FCustomCubeTransform ChairTransform
		{
			ChairTransform.Location = StartPosition + SequenceDirection * (DistanceTraveled - ChairSize.Y),
			ChairTransform.Rotation = { 0.0f, 0.0f, Rotation + CustomTransform.Rotation.Z},
			ChairTransform.Size = ChairSize
		};

		DrawChair(ChairTransform);

		DistanceTraveled += ChairSize.Y * 2 + DistanceBetweenChairs;
	}
}

FCustomCubeMeshData UCustomTableComponent::DrawChair(const FCustomCubeTransform& Transform)
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

FCustomCubeMeshData UCustomTableComponent::DrawFrontLeftLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize)
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

FCustomCubeMeshData UCustomTableComponent::DrawFrontRightLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize)
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

FCustomCubeMeshData UCustomTableComponent::DrawBackLeftLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize)
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

FCustomCubeMeshData UCustomTableComponent::DrawBackRightLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize)
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
