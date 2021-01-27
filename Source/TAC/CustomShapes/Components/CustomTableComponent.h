// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TAC/CustomLinecast/CustomRaycastTypes.h"
#include "TAC/CustomShapes/CustomShapesTypes.h"
#include "CustomTableComponent.generated.h"

UCLASS(hideCategories = "ProceduralMesh")
class TAC_API UCustomTableComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UCustomTableComponent(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(CallInEditor)
	void DebugDraw();
	FCustomCubeMeshData Draw();
	FCustomCubeMeshData Draw(const FVector& Location);

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

	const FCustomBoxRaycastCollider& GetCollider() const { return CustomBoxCollider; }
	FCustomBoxRaycastCollider& GetCollider() { return CustomBoxCollider; }

	const FCustomShapeBuffers& GetCustomShapeBuffer() const { return CustomShapeBuffers; }

	const FVector2D& GetTableMinSize() const { return TableMinSize; }

private:
	UPROPERTY(EditAnywhere, Category = "Custom transform")
	FCustomCubeTransform CustomTransform;

	UPROPERTY(EditAnywhere, Category = "Custom colliders")
	FCustomBoxRaycastCollider CustomBoxCollider;

	UPROPERTY(EditAnywhere, Category = "Table values")
	FVector2D TableLegsSize;

	UPROPERTY(EditAnywhere, Category = "Table values")
	FVector2D TableMinSize;

	UPROPERTY(EditAnywhere, Category = "Chairs values")
	float DistanceBetweenChairs;

	UPROPERTY(EditAnywhere, Category = "Chairs values")
	float ChairDistanceFromTableSide;

	UPROPERTY(EditAnywhere, Category = "Chairs values")
	float ChairDistanceFromTableBottom;

	UPROPERTY(EditAnywhere, Category = "Chairs values")
	FVector ChairSize;

	UPROPERTY(EditAnywhere, Category = "Chairs values")
	FVector2D ChairLegsSize;

	UPROPERTY(EditAnywhere, Category = "Chairs values")
	FVector ChairBackRestSize;

	UPROPERTY()
	FCustomShapeBuffers CustomShapeBuffers;

	FCustomCubeMeshData LastDrawnTable;
};

