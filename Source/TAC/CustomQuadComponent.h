// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomRaycastTypes.h"
#include "CustomShapesTypes.h"
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

protected:
	virtual void BeginPlay() override;

public:
	const FCustomQuadTransform& GetTransform() const { return CustomTransform; }
	FCustomQuadTransform& GetTransform() { return CustomTransform; }

	const FCustomBoxRaycastCollider* GetCollider() const { return &CustomBoxCollider; }
	FCustomBoxRaycastCollider* GetCollider() { return &CustomBoxCollider; }

private:
	UPROPERTY(EditAnywhere, Category = "Custom")
	FCustomQuadTransform CustomTransform;

	UPROPERTY(EditAnywhere, Category = "Custom")
	FCustomBoxRaycastCollider CustomBoxCollider;

	UPROPERTY(VisibleAnywhere, Category = "Custom")
	FCustomShapeBuffers CustomShapeBuffers;
};
