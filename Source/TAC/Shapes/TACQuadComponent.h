// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TAC/TAC.h"
#include "TACRenderSystemModule/Public/TACRenderShapesTypes.h"
#include "TACCollisionSystemModule/Public/TACColliders.h"
#include "TACQuadComponent.generated.h"

UCLASS(hideCategories = "ProceduralMesh", ClassGroup = Custom, meta = (BlueprintSpawnableComponent))
class TAC_API UTACQuadComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
public: 
	UTACQuadComponent(const FObjectInitializer& ObjectInitializer);

public:	
	UFUNCTION(CallInEditor)
	void Draw();
	void Draw(const FVector& Location, const FVector2D& Size);

	void GenerateCollider();
	void GenerateCollider(const FVector& Location, const FVector2D& Size);

public:
	const FTACQuadTransform& GetCustomTransform() const { return CustomTransform; }
	FTACQuadTransform& GetCustomTransform() { return CustomTransform; }

	const FTACBoxCollider& GetCollider() const { return CustomBoxCollider; }
	FTACBoxCollider& GetCollider() { return CustomBoxCollider; }

private:
	UPROPERTY(EditAnywhere, Category = "Custom transform")
	FTACQuadTransform CustomTransform;

	UPROPERTY(VisibleAnywhere, Category = "Custom colliders")
	FTACBoxCollider CustomBoxCollider;

	UPROPERTY()
	FTACShapeBuffers CustomShapeBuffers;
};
