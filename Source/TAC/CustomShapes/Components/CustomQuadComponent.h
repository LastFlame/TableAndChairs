// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TAC/CustomLinecast/CustomRaycastTypes.h"
#include "TAC/CustomShapes/CustomShapesTypes.h"
#include "CustomQuadComponent.generated.h"

UCLASS(hideCategories = "ProceduralMesh", ClassGroup = Custom, meta = (BlueprintSpawnableComponent))
class TAC_API UCustomQuadComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
public: 
	UCustomQuadComponent(const FObjectInitializer& ObjectInitializer);

public:	
	UFUNCTION(CallInEditor)
	void Draw();
	void Draw(const FVector& Location, const FVector2D& Size);

	void GenerateCollider();
	void GenerateCollider(const FVector& Location, const FVector2D& Size);

public:
	const FCustomQuadTransform& GetTransform() const { return CustomTransform; }
	FCustomQuadTransform& GetTransform() { return CustomTransform; }

	const FCustomBoxRaycastCollider* GetCollider() const { return &CustomBoxCollider; }
	FCustomBoxRaycastCollider* GetCollider() { return &CustomBoxCollider; }

private:
	UPROPERTY(EditAnywhere, Category = "Custom transform")
	FCustomQuadTransform CustomTransform;

	UPROPERTY(EditAnywhere, Category = "Custom colliders")
	FCustomBoxRaycastCollider CustomBoxCollider;

	UPROPERTY()
	FCustomShapeBuffers CustomShapeBuffers;
};