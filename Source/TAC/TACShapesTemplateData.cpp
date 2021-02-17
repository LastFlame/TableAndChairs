// Fill out your copyright notice in the Description page of Project Settings.


#include "TACShapesTemplateData.h"

UTACShapesTemplateData::UTACShapesTemplateData() :
	SphereRadius(8.0f), DragThreashold(5.0f), LocationBounds(500, 500),
	CustomTransform({ 0.0f, 0.0f, 73.0f }, { 0.0f, 0.0f, 0.0f }, { 35.0f, 35.0f, 1.5f }), ColliderMaxBoundOffset(20.0f), TableLegsSize(2.0f, 2.0f),
	TableMinSize(CustomTransform.Size.X, CustomTransform.Size.Y), DistanceBetweenChairs(10.0f), ChairDistanceFromTableSide(30.0f),
	ChairDistanceFromTableBottom(30.0f), ChairSize(20.0f, 18.0f, 1.0f), ChairLegsSize(1.0f, 1.0f), ChairBackRestSize(1.0f, 0.0f, 18.0f),
	TableTag("Table"), TableSpawnerTag("TableSpawner")
{
	const float DistanceFromMidToFrontBackRestChair = CustomTransform.Size.X + ChairDistanceFromTableSide+ ChairSize.X + ChairBackRestSize.X;
	const float DistanceFromtMidToRightBackRestChair = CustomTransform.Size.Y + ChairDistanceFromTableSide + ChairSize.Y + ChairBackRestSize.Y;

	const FVector MinStartLocation = CustomTransform.Location - FVector::UpVector * (CustomTransform.Location.Z);

	const FVector MaxStartLocation = CustomTransform.Location + FVector::UpVector * ColliderMaxBoundOffset;

	// Location: top left looking at the table after the chairs.
	DefaultTableBoundCollider.SetMaxBounds((MaxStartLocation + FVector::ForwardVector * DistanceFromMidToFrontBackRestChair)
		+ FVector::RightVector * DistanceFromtMidToRightBackRestChair);

	// Location: bottom right looking at the table after the chairs.
	DefaultTableBoundCollider.SetMinBounds((MinStartLocation + FVector::BackwardVector * DistanceFromMidToFrontBackRestChair)
		+ FVector::LeftVector * DistanceFromtMidToRightBackRestChair);

	DefaultTableBoundCollider.SetFlag(ETACCollisionFlags::Dynamic);
}
