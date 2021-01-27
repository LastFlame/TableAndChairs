// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TAC/CustomLinecast/CustomRaycastTypes.h"
#include "TAC/CustomShapes/CustomShapesTypes.h"
#include "CustomSphereComponent.generated.h"

UCLASS(hideCategories = "ProceduralMesh")
class TAC_API UCustomSphereComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor)
	void Draw();
	void Draw(const FVector& Location, float Radius);

	void GenerateCollider();
	void GenerateCollider(const FVector& Location, float Radius);

public:
	const FCustomSphereTransform& GetTransform() const { return CustomTransform; }
	FCustomSphereTransform& GetTransform() { return CustomTransform; }

	const FCustomSphereRaycastCollider& GetCollider() const { return CustomSphereCollider; }
	FCustomSphereRaycastCollider& GetCollider() { return CustomSphereCollider; }

private:
	UPROPERTY(EditAnywhere, Category = "Custom transform")
	FCustomSphereTransform CustomTransform;

	UPROPERTY(EditAnywhere, Category = "Custom colliders")
	FCustomSphereRaycastCollider CustomSphereCollider;

	UPROPERTY()
	FCustomShapeBuffers CustomShapeBuffers;
};
