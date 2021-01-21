// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomShapesTypes.h"
#include "CustomRaycastTypes.h"
#include "CustomTableComponent.h"
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
	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();

	bool Drag(const FCustomRaycastBaseCollider* Collider, const FVector& DragLocation);
	bool DragEdge(const FVector& ForwardDir, const FVector& RightDir, const FVector& DragDir, float ForwardDragDistance, float RightDragDistance);
	void ResetSelection();

public:
	virtual void OnHit(const FCustomRaycastBaseCollider* Collider, const FVector& HitPoint) override;
	virtual TWeakObjectPtr<AActor> GetActor() const override { return (AActor*)this; }
	virtual const FCustomRaycastBaseCollider* GetBoundCollider() const { return TableComponent->GetCollider();  }
	virtual const TArray<FCustomRaycastBaseCollider*>& GetColliders() const override { return RaycastColliders; }

	const FCustomCubeTransform& GetCustomTransform() const { return TableComponent->GetTransform(); }
	FCustomCubeTransform& GetCustomTransform() { return TableComponent->GetTransform(); }

	void SetCustomLocation(float X, float Y) { TableComponent->GetTransform().Location.X = X; TableComponent->GetTransform().Location.Y = Y; }

private:
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

	UPROPERTY(EditAnywhere, Category = "Mesh values")
	float SphereRadius = 8.0f;

	UPROPERTY(EditAnywhere, Category = "Mesh values")
	UMaterialInterface* SelectedSphereMaterial;

	UPROPERTY(EditAnywhere, Category = "Mesh values")
	UMaterialInterface* SelectedTableMaterial;

	UPROPERTY(EditAnywhere, Category = "Mesh values")
	float DragThreshold = 5.0f;

	UPROPERTY()
	UCustomSphereComponent* HitSphere;

	TArray<FCustomRaycastBaseCollider*> RaycastColliders;
};
