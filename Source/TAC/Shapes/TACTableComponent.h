// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TAC/TAC.h"
#include "TACRenderSystemModule/Public/TACRenderShapesTypes.h"
#include "TACCollisionSystemModule/Public/TACColliders.h"
#include "TACTableComponent.generated.h"

class UTACShapesTemplateData;

// Table and chairs are a single procedural mesh to maintain only one draw call for table.
// This optimization prevent to use multiple materials for the table and the chairs.

UCLASS(hideCategories = "ProceduralMesh")
class TAC_API UTACTableComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UTACTableComponent(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(CallInEditor)
	void DebugDraw();
	FTACCubeMeshData Draw();
	FTACCubeMeshData Draw(const FVector& Location);

	bool CreateCollider(const FTACCubeTransform& Transform, FTACBoxCollider& OutBoxCollider) const;
	void GenerateCollider();
	void GenerateCollider(const FVector& MinBounds, const FVector& MaxBounds);

private:
	FTACCubeMeshData DrawTable(const FTACCubeTransform& Transform, const FVector2D& LegsSize);
	void DrawChairs(const FTACCubeMeshData& TableCustomVertices);
	void DrawSequenceOfChairs(const FVector& TableRightCorner, const FVector& TableLeftCorner, const FVector& OffsetDirection, float Rotation);

	FTACCubeMeshData DrawChair(const FTACCubeTransform& Transform);

	FTACCubeMeshData DrawFrontLeftLeg(const FTACCubeTransform& Transform, const FTACCubeMeshData& VerticesData, const FVector2D& LegsSize);
	FTACCubeMeshData DrawFrontRightLeg(const FTACCubeTransform& Transform, const FTACCubeMeshData& VerticesData, const FVector2D& LegsSize);

	FTACCubeMeshData DrawBackLeftLeg(const FTACCubeTransform& Transform, const FTACCubeMeshData& VerticesData, const FVector2D& LegsSize);
	FTACCubeMeshData DrawBackRightLeg(const FTACCubeTransform& Transform, const FTACCubeMeshData& VerticesData, const FVector2D& LegsSize);

public:
	const FTACCubeTransform& GetTransform() const { return CustomTransform; }
	FTACCubeTransform& GetTransform() { return CustomTransform; }

	const FTACBoxCollider& GetCollider() const { return CustomBoxCollider; }
	FTACBoxCollider& GetCollider() { return CustomBoxCollider; }

	const FTACShapeBuffers& GetCustomShapeBuffer() const { return CustomShapeBuffers; }

	const FVector2D& GetTableMinSize() const { return TableMinSize; }

private:
	UPROPERTY(EditAnywhere, Category = "Custom transform")
	FTACCubeTransform CustomTransform;

	UPROPERTY(VisibleAnywhere, Category = "Custom colliders")
	FTACBoxCollider CustomBoxCollider;

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
	UTACShapesTemplateData* CustomShapeTemplateData;

	UPROPERTY()
	FTACShapeBuffers CustomShapeBuffers;

	FTACCubeMeshData LastDrawnTable;
};

