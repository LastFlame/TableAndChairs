// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TAC/CustomShapes/CustomShapesTypes.h"
#include "TACCollisionSystemModule/Public/TACColliders.h"
#include "CustomShapeTemplateDataAsset.generated.h"

UCLASS()
class TAC_API UCustomShapeTemplateDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UCustomShapeTemplateDataAsset();

public:
	float GetSphereRadius() const { return SphereRadius; }
	float GetDragThreashold() const { return DragThreashold; }
	const FVector2D& GetLocationBounds() const { return LocationBounds; }
	
	const FCustomCubeTransform& GetCustomTransform() const { return CustomTransform; }

	const FTACBoxCollider& GetCustomBoxCollider() const { return CustomBoxCollider; }
	const FTACBoxCollider& GetDefaultTableBoundCollider() const { return DefaultTableBoundCollider; }
	float GetColliderMaxBoundOffset() const { return ColliderMaxBoundOffset; }

	const FVector2D& GetTableLegsSize() const { return TableLegsSize; }
	const FVector2D& GetTableMinSize() const { return TableMinSize; }

	float GetDistanceBetweenCharis() const { return DistanceBetweenChairs; }
	float GetChairDistanceFromTableSide() const { return ChairDistanceFromTableSide; }
	float GetChairDistanceFromTableBottom() const { return ChairDistanceFromTableBottom; }

	const FVector& GetChairSize() const { return ChairSize; }
	const FVector2D& GetChairLegsSize() const { return ChairLegsSize; }
	const FVector& GetChairBackRestSize() const { return ChairBackRestSize; }

private:
	UPROPERTY(EditAnywhere, Category = "CustomShapeActor|Sphere values")
	float SphereRadius;

	UPROPERTY(EditAnywhere, Category = "CustomShapeActor|Sphere values")
	float DragThreashold;

	UPROPERTY(EditAnywhere, Category = "CustomShapeActor|Sphere values")
	FVector2D LocationBounds;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Custom transform")
	FCustomCubeTransform CustomTransform;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Custom colliders")
	FTACBoxCollider CustomBoxCollider;

	UPROPERTY(VisibleAnywhere, Category = "TableComponent|Custom colliders")
	FTACBoxCollider DefaultTableBoundCollider;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Custom colliders")
	float ColliderMaxBoundOffset;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Table values")
	FVector2D TableLegsSize;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Table values")
	FVector2D TableMinSize;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Chairs values")
	float DistanceBetweenChairs;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Chairs values")
	float ChairDistanceFromTableSide;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Chairs values")
	float ChairDistanceFromTableBottom;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Chairs values")
	FVector ChairSize;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Chairs values")
	FVector2D ChairLegsSize;

	UPROPERTY(EditAnywhere, Category = "TableComponent|Chairs values")
	FVector ChairBackRestSize;
};
