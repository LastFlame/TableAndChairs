// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TAC/CustomShapes/CustomShapesTypes.h"
#include "TACCollisionSystemModule/Public/TACColliders.h"
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

	const FTACSphereCollider& GetCollider() const { return CustomSphereCollider; }
	FTACSphereCollider& GetCollider() { return CustomSphereCollider; }

private:
	UPROPERTY(EditAnywhere, Category = "Custom transform")
	FCustomSphereTransform CustomTransform;

	UPROPERTY(VisibleAnywhere, Category = "Custom colliders")
	FTACSphereCollider CustomSphereCollider;

	UPROPERTY()
	FCustomShapeBuffers CustomShapeBuffers;
};
