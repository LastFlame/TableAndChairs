// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomShapesTypes.h"
#include "ComponentVisualizer.h"
#include "CustomRaycastTypes.h"
#include "CustomShape.generated.h"

class USceneComponent;
class UCustomSphereComponent;

UCLASS()
class TAC_API ACustomShape : public AActor, public ICustomRaycastHittable
{
	GENERATED_BODY()

public:
	ACustomShape();

public:
	bool Drag(const FCustomRaycastBaseCollider* Collider, const FVector& DragLocation);
	bool DragEdge(const FVector& ForwardDir, const FVector& RightDir, const FVector& DragDir, float ForwardDragDistance, float RightDragDistance);

	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();

	void ResetSelection();

public:
	virtual void OnHit(const FCustomRaycastBaseCollider* Collider, const FVector& HitPoint) override;
	virtual TWeakObjectPtr<AActor> GetActor() const override { return (AActor*)this; }
	virtual const FCustomRaycastBaseCollider* GetBoundCollider() const { return &BoundBoxCollider;  }
	virtual const TArray<FCustomRaycastBaseCollider*>& GetColliders() const override { return RaycastColliders; }

	const FCustomCubeTransform& GetCustomTransform() const { return TableTransform; }
	FCustomCubeTransform& GetCustomTransform() { return TableTransform; }

	void SetCustomLocation(float X, float Y) { TableTransform.Location.X = X; TableTransform.Location.Y = Y; }

private:
	FCustomCubeMeshData DrawTable(const FCustomCubeTransform& Transform, const FVector2D& LegsSize);
	void DrawChairs(const FCustomCubeMeshData& TableCustomVertices);
	void DrawSequenceOfChairs(const FVector& TableRightCorner, const FVector& TableLeftCorner, const FVector& OffsetDirection, float Rotation);

	FCustomCubeMeshData DrawChair(const FCustomCubeTransform& Transform);

	FCustomCubeMeshData DrawFrontLeftLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize);
	FCustomCubeMeshData DrawFrontRightLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize);

	FCustomCubeMeshData DrawBackLeftLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize);
	FCustomCubeMeshData DrawBackRightLeg(const FCustomCubeTransform& Transform, const FCustomCubeMeshData& VerticesData, const FVector2D& LegsSize);

private:
	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UProceduralMeshComponent* TableAndChairsMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomSphereComponent* TopRightSphere;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomSphereComponent* BottomRightSphere;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomSphereComponent* TopLeftSphere;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomSphereComponent* BottomLeftSphere;

	UPROPERTY(EditAnywhere, Category = "Table values")
	FCustomCubeTransform TableTransform;

	UPROPERTY(EditAnywhere, Category = "Table values")
	FVector2D TableLegsSize;

	UPROPERTY(EditAnywhere, Category = "Table values")
	FVector2D TableMinSize;

	UPROPERTY(EditAnywhere, Category = "Table values")
	FCustomBoxRaycastCollider BoundBoxCollider;

	UPROPERTY(EditAnywhere, Category = "Chair values")
	float DistanceBetweenChairs = 10.f;

	UPROPERTY(EditAnywhere, Category = "Chair values")
	float ChairDistanceFromTableSide = 30.0f;
	
	UPROPERTY(EditAnywhere, Category = "Chair values")
	float ChairDistanceFromTableBottom = 30.f;

	UPROPERTY(EditAnywhere, Category = "Chair values")
	FVector ChairSize = FVector(20.0f, 18.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Chair values")
	FVector2D ChairLegsSize = FVector2D(1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Chair values")
	FVector ChairBackRestSize = FVector(1.0f, 0.0f, 18.0f);

	UPROPERTY(EditAnywhere, Category = "Mesh values")
	float SphereRadius = 8.0f;

	//UPROPERTY(EditAnywhere, Category = "Mesh values")
	UPROPERTY()
	FCustomShapeBuffers TableAndChairsCustomShapeBuffers;

	TArray<FCustomRaycastBaseCollider*> RaycastColliders;

	UPROPERTY(EditAnywhere, Category = "Mesh values")
	UMaterialInterface* SelectedSphereMaterial;

	UPROPERTY(EditAnywhere, Category = "Mesh values")
	UMaterialInterface* SelectedTableMaterial;

	UPROPERTY(EditAnywhere, Category = "Mesh values")
	float DragThreshold = 5.0f;

	UPROPERTY()
	UCustomSphereComponent* HitSphere;
};
