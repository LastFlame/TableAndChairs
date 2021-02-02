// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TAC/CustomCollisions/CustomColliders.h"
#include "TAC/CustomShapes/CustomShapesTypes.h"
#include "CustomTableComponent.generated.h"

class UCustomShapeTemplateDataAsset;

//struct CreateColliderData
//{
//	FCustomCubeTransform Transform;
//
//	FVector ChairSize, ChairBackRestSize;
//	FVector Up, Forward, Right;
//
//	float ChairDistanceFromTableSide, ColliderMaxBoundOffset;
//};

UCLASS(hideCategories = "ProceduralMesh")
class TAC_API UCustomTableComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UCustomTableComponent(const FObjectInitializer& ObjectInitializer);

public:
	//static FCustomBoxCollider CreateCollider(const CreateColliderData& ColliderData);

	UFUNCTION(CallInEditor)
	void DebugDraw();
	FCustomCubeMeshData Draw();
	FCustomCubeMeshData Draw(const FVector& Location);

	bool CreateCollider(const FCustomCubeTransform& Transform, FCustomBoxCollider& OutBoxCollider) const;
	void GenerateCollider();
	void GenerateCollider(const FVector& MinBounds, const FVector& MaxBounds);

private:
	FCustomCubeMeshData DrawTable(const FCustomCubeTransform& Transform, const FVector2D& LegsSize);
	void DrawChairs(const FCustomCubeMeshData& TableCustomVertices);
	void DrawSequenceOfChairs(const FVector& TableRightCorner, const FVector& TableLeftCorner, const FVector& OffsetDirection, float Rotation);

	FCustomCubeMeshData DrawChair(const FCustomCubeTransform& Transform);

	FCustomCubeMeshData DrawFrontLeftLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize);
	FCustomCubeMeshData DrawFrontRightLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize);

	FCustomCubeMeshData DrawBackLeftLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize);
	FCustomCubeMeshData DrawBackRightLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize);

public:
	const FCustomCubeTransform& GetTransform() const { return CustomTransform; }
	FCustomCubeTransform& GetTransform() { return CustomTransform; }

	const FCustomBoxCollider& GetCollider() const { return CustomBoxCollider; }
	FCustomBoxCollider& GetCollider() { return CustomBoxCollider; }

	const FCustomShapeBuffers& GetCustomShapeBuffer() const { return CustomShapeBuffers; }

	const FVector2D& GetTableMinSize() const { return TableMinSize; }

private:
	UPROPERTY(EditAnywhere, Category = "Custom transform")
	FCustomCubeTransform CustomTransform;

	UPROPERTY(VisibleAnywhere, Category = "Custom colliders")
	FCustomBoxCollider CustomBoxCollider;

	UPROPERTY(VisibleAnywhere, Category = "Custom colliders")
	float ColliderMaxBoundOffset;

	UPROPERTY(VisibleAnywhere, Category = "Table values")
	FVector2D TableLegsSize;

	UPROPERTY(VisibleAnywhere, Category = "Table values")
	FVector2D TableMinSize;

	UPROPERTY(VisibleAnywhere, Category = "Chairs values")
	float DistanceBetweenChairs;

	UPROPERTY(VisibleAnywhere, Category = "Chairs values")
	float ChairDistanceFromTableSide;

	UPROPERTY(VisibleAnywhere, Category = "Chairs values")
	float ChairDistanceFromTableBottom;

	UPROPERTY(VisibleAnywhere, Category = "Chairs values")
	FVector ChairSize;

	UPROPERTY(VisibleAnywhere, Category = "Chairs values")
	FVector2D ChairLegsSize;

	UPROPERTY(VisibleAnywhere, Category = "Chairs values")
	FVector ChairBackRestSize;

	UPROPERTY()
	UCustomShapeTemplateDataAsset* CustomShapeTemplateData;

	UPROPERTY()
	FCustomShapeBuffers CustomShapeBuffers;

	FCustomCubeMeshData LastDrawnTable;
};

