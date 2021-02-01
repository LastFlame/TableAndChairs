// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAC/CustomShapes/CustomShapesTypes.h"
#include "TAC/CustomCollisions/CustomColliders.h"
#include "TAC/CustomShapes/Components/CustomTableComponent.h"
#include "CustomShape.generated.h"

class USceneComponent;
class UCustomSphereComponent;
class UCustomShapeTemplateDataAsset;

UCLASS()
class TAC_API ACustomShape : public AActor, public ICustomHittable
{
	GENERATED_BODY()

public:
	ACustomShape();

public:
	static void Create(UWorld* World, const FVector& Location);

	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();

	bool Drag(const FCustomBaseCollider& Collider, const FVector& DragLocation);
	bool DragEdge(const FVector& ForwardDir, const FVector& RightDir, const FVector& DragDir, float ForwardDragDistance, float RightDragDistance);

public:
	/* ICustomRaycastHittable interface */
	virtual const FCustomBaseCollider& GetBoundCollider() const { return TableComponent->GetCollider();  }
	virtual const CustomCollidersArray& GetColliders() const override { return RaycastCollidersArray; }
	/***********************************/

public:
	const FCustomCubeTransform& GetCustomTransform() const { return TableComponent->GetTransform(); }
	FCustomCubeTransform& GetCustomTransform() { return TableComponent->GetTransform(); }
	void SetCustomLocation(float X, float Y) { TableComponent->GetTransform().Location.X = X; TableComponent->GetTransform().Location.Y = Y; }

private:
	UFUNCTION()
	void OnBoundColliderHit(const FVector& HitPoint);

	UFUNCTION()
	void OnBoundColliderHitChanged();

	UFUNCTION()
	void OnTopRightSphereHit(const FVector& HitPoint);

	UFUNCTION()
	void OnBottomRightSphereHit(const FVector& HitPoint);

	UFUNCTION()
	void OnTopLeftSphereHit(const FVector& HitPoint);

	UFUNCTION()
	void OnBottomLeftSphereHit(const FVector& HitPoint);

	UFUNCTION(CallInEditor, Category = "Debug")
	void ShowDebugBoxCollider() const;

private:
	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomTableComponent* TableComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomSphereComponent* TopRightSphereComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomSphereComponent* BottomRightSphereComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomSphereComponent* TopLeftSphereComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomSphereComponent* BottomLeftSphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh values")
	float SphereRadius;

	UPROPERTY(VisibleAnywhere, Category = "Mesh values")
	float DragThreshold;

	UPROPERTY(VisibleAnywhere, Category = "Mesh values")
	UMaterialInterface* OnSphereSelectedMat;

	UPROPERTY(VisibleAnywhere, Category = "Mesh values")
	UMaterialInterface* OnSelectedTableMat;

	UPROPERTY()
	UCustomShapeTemplateDataAsset* CustomShapeTemplateData;

	UPROPERTY()
	UCustomSphereComponent* PrevHitSphere;

	CustomCollidersArray RaycastCollidersArray;
};
