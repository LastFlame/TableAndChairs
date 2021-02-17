// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAC/CustomShapes/Components/TACTableComponent.h"
#include "TACCollisionSystemModule/Public/TACColliders.h"
#include "TACTableShape.generated.h"

class USceneComponent;
class UTACSphereComponent;
class UTACShapesTemplateData;

UCLASS()
class TAC_API ATACTableShape : public AActor, public ITACHittable
{
	GENERATED_BODY()

public:
	ATACTableShape();

public:
	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();

	bool Drag(const FTACBaseCollider& Collider, const FVector& DragLocation);
	bool DragEdge(const FVector& ForwardDir, const FVector& RightDir, const FVector& DragDir, float ForwardDragDistance, float RightDragDistance);

	bool Move(const FVector& Location);

	void ResetDraggableSphere();

public:
	/* ICustomRaycastHittable interface */
	virtual TWeakObjectPtr<AActor> GetActor() const  override { return (AActor*)this; }
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
	UTACTableComponent* TableComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UTACSphereComponent* TopRightSphereComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UTACSphereComponent* BottomRightSphereComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UTACSphereComponent* TopLeftSphereComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UTACSphereComponent* BottomLeftSphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh values")
	float SphereRadius;

	UPROPERTY(VisibleAnywhere, Category = "Mesh values")
	float DragThreshold;

	UPROPERTY(VisibleAnywhere, Category = "Mesh values")
	UMaterialInterface* OnSphereSelectedMat;

	UPROPERTY(VisibleAnywhere, Category = "Mesh values")
	UMaterialInterface* OnSelectedTableMat;

	UPROPERTY()
	UTACShapesTemplateData* CustomShapeTemplateData;

	UPROPERTY()
	UTACSphereComponent* PrevHitSphere;

	TACCollidersArray RaycastCollidersArray;
};