// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAC/CustomShapes/Components/CustomQuadComponent.h"
#include "TACCollisionSystemModule/Public/TACColliders.h"

#include "CustomGround.generated.h"

UCLASS()
class TAC_API ACustomGround : public AActor, public ITACHittable
{
	GENERATED_BODY()
	
public:	
	ACustomGround();

public:
	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();

public:
	/* ICustomRaycastHittable interface */
	virtual const FTACBaseCollider& GetBoundCollider() const override { return QuadComponent->GetCollider(); }
	virtual const TACCollidersArray& GetColliders() const override { return RaycastCollidersArray; }
	/***********************************/

private:
	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomQuadComponent* QuadComponent;

	TACCollidersArray RaycastCollidersArray;
};
