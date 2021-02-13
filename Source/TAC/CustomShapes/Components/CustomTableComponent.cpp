// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomTableComponent.h"
#include "TACRenderSystemModule/Public/TACRenderSystem.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "TAC/CustomShapeTemplateDataAsset.h"


UCustomTableComponent::UCustomTableComponent(const FObjectInitializer& ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UCustomShapeTemplateDataAsset> CustomShapeDataAsset(TEXT("CustomShapeTemplateDataAsset'/Game/TAC/CustomShapeTemplateDataAsset.CustomShapeTemplateDataAsset'"));
	if (CustomShapeDataAsset.Object != NULL)
	{
		CustomShapeTemplateData = CustomShapeDataAsset.Object;
		CustomTransform = CustomShapeTemplateData->GetCustomTransform();
		ColliderMaxBoundOffset = CustomShapeTemplateData->GetColliderMaxBoundOffset();
		TableLegsSize = CustomShapeTemplateData->GetTableLegsSize();
		TableMinSize = CustomShapeTemplateData->GetTableMinSize();
		DistanceBetweenChairs = CustomShapeTemplateData->GetDistanceBetweenCharis();
		ChairDistanceFromTableSide = CustomShapeTemplateData->GetChairDistanceFromTableSide();
		ChairDistanceFromTableBottom = CustomShapeTemplateData->GetChairDistanceFromTableBottom();
		ChairSize = CustomShapeTemplateData->GetChairSize();
		ChairLegsSize = CustomShapeTemplateData->GetChairLegsSize();
		ChairBackRestSize = CustomShapeTemplateData->GetChairBackRestSize();
	}
	
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

FTACCubeMeshData UCustomTableComponent::Draw()
{
	// Prevents the table size from becoming negative and containing less than 1 chair.
	CustomTransform.Size.X = FGenericPlatformMath::Max(CustomTransform.Size.X, TableMinSize.X);
	CustomTransform.Size.Y = FGenericPlatformMath::Max(CustomTransform.Size.Y, TableMinSize.Y);

	CustomShapeBuffers.Reset();
	LastDrawnTable.Normals = nullptr;

	TACRender::BeginScene(CustomShapeBuffers, *this);
	{
		LastDrawnTable = DrawTable(CustomTransform, TableLegsSize);
		DrawChairs(LastDrawnTable);
	}
	TACRender::EndScene();

	return LastDrawnTable;
}

FTACCubeMeshData UCustomTableComponent::Draw(const FVector& Location)
{
	CustomTransform.Location = Location;
	return Draw();
}

bool UCustomTableComponent::CreateCollider(const FTACCubeTransform& Transform, FTACBoxCollider& OutBoxCollider) const
{
	FTACCubeQuads* TableNormals = nullptr;

	if (LastDrawnTable.Normals != nullptr)
	{
		TableNormals = LastDrawnTable.Normals;
	}
	else if (CustomShapeBuffers.NormalsBuffer.Num() != 0) // Fallback in case the mesh as been drawn but LastDrawnTable is null.
	{
		TableNormals = (FTACCubeQuads*)(&CustomShapeBuffers.NormalsBuffer[0]); // Mimic the return value of CustomShapesRenderer::DrawCube().
	}

	if (TableNormals == nullptr)
	{
		return false;
	}

	const FVector& TableTopNormal = TableNormals->TopQuad.TopRight;
	const FVector& TableFrontNormal = TableNormals->FrontQuad.TopRight;
	const FVector& TableRightNormal = TableNormals->RightQuad.TopRight;
	const FVector& TableLeftNormal = TableNormals->LeftQuad.TopRight;
	const FVector& TableBackNormal = TableNormals->BackQuad.TopRight;

	const float DistanceFromMidToFrontBackRestChair = Transform.Size.X + ChairDistanceFromTableSide + ChairSize.X + ChairBackRestSize.X + 5.0f;
	const float DistanceFromtMidToRightBackRestChair = Transform.Size.Y + ChairDistanceFromTableSide + ChairSize.Y + ChairBackRestSize.Y + 5.0f;
	const FVector MinStartLocation = Transform.Location - TableTopNormal * (Transform.Location.Z);
	const FVector MaxStartLocation = Transform.Location + TableTopNormal * ColliderMaxBoundOffset;

	OutBoxCollider.SetLocation(Transform.Location);

	// Location: top left looking at the table after the chairs.
	OutBoxCollider.SetMaxBounds((MaxStartLocation + TableFrontNormal * DistanceFromMidToFrontBackRestChair)
		+ TableRightNormal * DistanceFromtMidToRightBackRestChair);

	// Location: bottom right looking at the table after the chairs.
	OutBoxCollider.SetMinBounds((MinStartLocation + TableBackNormal * DistanceFromMidToFrontBackRestChair)
		+ TableLeftNormal * DistanceFromtMidToRightBackRestChair);

	return true;
}

void UCustomTableComponent::GenerateCollider()
{
	FTACCubeQuads* TableNormals = nullptr;

	if (LastDrawnTable.Normals != nullptr)
	{
		TableNormals = LastDrawnTable.Normals;
	}
	else if (CustomShapeBuffers.NormalsBuffer.Num() != 0) // Fallback in case the mesh as been drawn but LastDrawnTable is null.
	{
		TableNormals = (FTACCubeQuads*)(&CustomShapeBuffers.NormalsBuffer[0]); // Mimic the return value of CustomShapesRenderer::DrawCube().
	}

	if (TableNormals == nullptr)
	{
		return;
	}
	
	CreateCollider(CustomTransform, CustomBoxCollider);
}

void UCustomTableComponent::GenerateCollider(const FVector& MinBounds, const FVector& MaxBounds)
{
	CustomBoxCollider.SetMinBounds(MinBounds);
	CustomBoxCollider.SetMaxBounds(MaxBounds);
}

FTACCubeMeshData UCustomTableComponent::DrawTable(const FTACCubeTransform& Transform, const FVector2D& LegsSize)
{
	FTACCubeMeshData TableVerticesData = TACRender::DrawCube(Transform);
	DrawFrontLeftLeg(Transform, TableVerticesData, LegsSize);
	DrawFrontRightLeg(Transform, TableVerticesData, LegsSize);
	DrawBackLeftLeg(Transform, TableVerticesData, LegsSize);
	DrawBackRightLeg(Transform, TableVerticesData, LegsSize);

	return TableVerticesData;
}

void UCustomTableComponent::DrawChairs(const FTACCubeMeshData& TableCustomVertices)
{
	// FRONT CHAIRS
	const FTACQuadVertices& TableFrontQuad = TableCustomVertices.Positions->FrontQuad;
	const FVector& TableFrontOffset = TableCustomVertices.Normals->FrontQuad.TopRight;
	DrawSequenceOfChairs(TableFrontQuad.BottomRight, TableFrontQuad.BottomLeft, TableFrontOffset, 180.0f);

	// LEFT CHARIS
	const FTACQuadVertices& TableLeftQuad = TableCustomVertices.Positions->LeftQuad;
	const FVector& TableLeftOffset = TableCustomVertices.Normals->LeftQuad.TopRight;
	DrawSequenceOfChairs(TableLeftQuad.BottomRight, TableLeftQuad.BottomLeft, TableLeftOffset, 90.0f);

	// BACK CHAIRS
	const FTACQuadVertices& TableBackQuad = TableCustomVertices.Positions->BackQuad;
	const FVector& TableBackOffset = TableCustomVertices.Normals->BackQuad.TopRight;
	DrawSequenceOfChairs(TableBackQuad.BottomRight, TableBackQuad.BottomLeft, TableBackOffset, 0.0f);

	// RIGHT CHARIS
	const FTACQuadVertices& TableRightQuad = TableCustomVertices.Positions->RightQuad;
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
		FTACCubeTransform ChairTransform
		{
			ChairTransform.Location = StartPosition + SequenceDirection * (DistanceTraveled - ChairSize.Y),
			ChairTransform.Rotation = { 0.0f, 0.0f, Rotation + CustomTransform.Rotation.Z},
			ChairTransform.Size = ChairSize
		};

		DrawChair(ChairTransform);

		DistanceTraveled += ChairSize.Y * 2 + DistanceBetweenChairs;
	}
}

FTACCubeMeshData UCustomTableComponent::DrawChair(const FTACCubeTransform& Transform)
{
	FTACCubeMeshData ChairSeat = DrawTable(Transform, ChairLegsSize);

	const FVector& ChairForwardDir = ChairSeat.Normals->FrontQuad.TopLeft;
	const FVector& ChairLeftDir = ChairSeat.Normals->RightQuad.TopLeft;
	const FVector& ChairUpDir = ChairSeat.Normals->TopQuad.TopLeft;

	FTACCubeTransform ChairBackRestTransform;
	ChairBackRestTransform.Location = ChairSeat.Positions->TopQuad.TopLeft;
	ChairBackRestTransform.Location += ChairForwardDir * ChairBackRestSize.X;
	ChairBackRestTransform.Location += ChairLeftDir * Transform.Size.Y;
	ChairBackRestTransform.Location += ChairUpDir * ChairBackRestSize.Z;

	ChairBackRestTransform.Rotation = Transform.Rotation;

	ChairBackRestTransform.Size.X = ChairBackRestSize.X;
	ChairBackRestTransform.Size.Y = Transform.Size.Y;
	ChairBackRestTransform.Size.Z = ChairBackRestSize.Z;

	TACRender::DrawCube(ChairBackRestTransform);

	return ChairSeat;
}

FTACCubeMeshData UCustomTableComponent::DrawFrontLeftLeg(const FTACCubeTransform& Transform, const FTACCubeMeshData& VerticesData, const FVector2D& LegsSize)
{
	const float Height = (Transform.Location.Z - Transform.Size.Z) * 0.5f;

	const FVector& ForwardDir = VerticesData.Normals->FrontQuad.TopLeft;
	const FVector& LeftDir = VerticesData.Normals->RightQuad.TopLeft;
	const FVector& UpDir = VerticesData.Normals->TopQuad.TopLeft;

	FTACCubeTransform LegTransform;
	LegTransform.Location = VerticesData.Positions->BottomQuad.BottomRight;
	LegTransform.Location -= ForwardDir * LegsSize.X;
	LegTransform.Location += LeftDir * LegsSize.Y;
	LegTransform.Location -= UpDir * Height;

	LegTransform.Rotation = Transform.Rotation;

	LegTransform.Size.X = LegsSize.X;
	LegTransform.Size.Y = LegsSize.Y;
	LegTransform.Size.Z = Height;

	return TACRender::DrawCube(LegTransform);
}

FTACCubeMeshData UCustomTableComponent::DrawFrontRightLeg(const FTACCubeTransform& Transform, const FTACCubeMeshData& VerticesData, const FVector2D& LegsSize)
{
	const float Height = (Transform.Location.Z - Transform.Size.Z) * 0.5f;

	const FVector& ForwardDir = VerticesData.Normals->FrontQuad.TopLeft;
	const FVector& LeftDir = VerticesData.Normals->RightQuad.TopLeft;
	const FVector& UpDir = VerticesData.Normals->TopQuad.TopLeft;

	FTACCubeTransform LegTransform;
	LegTransform.Location = VerticesData.Positions->BottomQuad.BottomLeft;
	LegTransform.Location -= ForwardDir * LegsSize.X;
	LegTransform.Location -= LeftDir * LegsSize.Y;
	LegTransform.Location -= UpDir * Height;

	LegTransform.Rotation = Transform.Rotation;

	LegTransform.Size.X = LegsSize.X;
	LegTransform.Size.Y = LegsSize.Y;
	LegTransform.Size.Z = Height;

	return TACRender::DrawCube(LegTransform);
}

FTACCubeMeshData UCustomTableComponent::DrawBackLeftLeg(const FTACCubeTransform& Transform, const FTACCubeMeshData& VerticesData, const FVector2D& LegsSize)
{
	const float Height = (Transform.Location.Z - Transform.Size.Z) * 0.5f;

	const FVector& ForwardDir = VerticesData.Normals->FrontQuad.TopLeft;
	const FVector& LeftDir = VerticesData.Normals->RightQuad.TopLeft;
	const FVector& UpDir = VerticesData.Normals->TopQuad.TopLeft;

	FTACCubeTransform LegTransform;
	LegTransform.Location = VerticesData.Positions->BottomQuad.TopRight;
	LegTransform.Location += ForwardDir * LegsSize.X;
	LegTransform.Location += LeftDir * LegsSize.Y;
	LegTransform.Location -= UpDir * Height;

	LegTransform.Rotation = Transform.Rotation;

	LegTransform.Size.X = LegsSize.X;
	LegTransform.Size.Y = LegsSize.Y;
	LegTransform.Size.Z = Height;

	return TACRender::DrawCube(LegTransform);
}

FTACCubeMeshData UCustomTableComponent::DrawBackRightLeg(const FTACCubeTransform& Transform, const FTACCubeMeshData& VerticesData, const FVector2D& LegsSize)
{
	const float Height = (Transform.Location.Z - Transform.Size.Z) * 0.5f;

	const FVector& ForwardDir = VerticesData.Normals->FrontQuad.TopLeft;
	const FVector& LeftDir = VerticesData.Normals->RightQuad.TopLeft;
	const FVector& UpDir = VerticesData.Normals->TopQuad.TopLeft;

	FTACCubeTransform LegTransform;
	LegTransform.Location = VerticesData.Positions->BottomQuad.TopLeft;
	LegTransform.Location += ForwardDir * LegsSize.X;
	LegTransform.Location -= LeftDir * LegsSize.Y;
	LegTransform.Location -= UpDir * Height;

	LegTransform.Rotation = Transform.Rotation;

	LegTransform.Size.X = LegsSize.X;
	LegTransform.Size.Y = LegsSize.Y;
	LegTransform.Size.Z = Height;

	return TACRender::DrawCube(LegTransform);
}
