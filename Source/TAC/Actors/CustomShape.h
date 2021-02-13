// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAC/CustomShapes/Components/CustomTableComponent.h"
#include "TACCollisionSystemModule/Public/TACColliders.h"
#include "CustomShape.generated.h"

class USceneComponent;
class UCustomSphereComponent;
class UCustomShapeTemplateDataAsset;

UCLASS()
class TAC_API ACustomShape : public AActor, public ITACHittable
{
	GENERATED_BODY()

public:
	ACustomShape();

public:
	static void Create(UWorld* World, const FVector& Location);

	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();

	bool Drag(const FTACBaseCollider& Collider, const FVector& DragLocation);
	bool DragEdge(const FVector& ForwardDir, const FVector& RightDir, const FVector& DragDir, float ForwardDragDistance, float RightDragDistance);

	bool Move(const FVector& Location);

	void ResetDraggableSphere();

public:
	/* ICustomRaycastHittable interface */
	virtual const FTACBaseCollider& GetBoundCollider() const { return TableComponent->GetCollider();  }
	virtual const TACCollidersArray& GetColliders() const override { return RaycastCollidersArray; }
	/***********************************/

public:
	const FTACCubeTransform& GetCustomTransform() const { return TableComponent->GetTransform(); }
	FTACCubeTransform& GetCustomTransform() { return TableComponent->GetTransform(); }
	void SetCustomLocation(float X, float Y) { TableComponent->GetTransform().Location.X = X; TableComponent->GetTransform().Location.Y = Y; }

protected:
	virtual void BeginPlay() override;

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

	bool IsBetweenLocationBounds(const FTACBoxCollider& BoxCollider) const;

	void ShowDebugBoxCollider(const FTACBoxCollider& BoxCollider, const FColor& Color) const;

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

	UPROPERTY(VisibleAnywhere, Category = "Mesh values")
	UMaterialInterface* OnMovingTableMat;

	UPROPERTY()
	UCustomShapeTemplateDataAsset* CustomShapeTemplateData;

	UPROPERTY()
	UCustomSphereComponent* PrevHitSphere;

	TACCollidersArray RaycastCollidersArray;
};
