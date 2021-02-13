// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TACRenderSystemModule/Public/TACRenderShapesTypes.h"
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
	const FTACSphereTransform& GetTransform() const { return CustomTransform; }
	FTACSphereTransform& GetTransform() { return CustomTransform; }

	const FTACSphereCollider& GetCollider() const { return CustomSphereCollider; }
	FTACSphereCollider& GetCollider() { return CustomSphereCollider; }

private:
	UPROPERTY(EditAnywhere, Category = "Custom transform")
	FTACSphereTransform CustomTransform;

	UPROPERTY(VisibleAnywhere, Category = "Custom colliders")
	FTACSphereCollider CustomSphereCollider;

	UPROPERTY()
	FTACShapeBuffers CustomShapeBuffers;
};
