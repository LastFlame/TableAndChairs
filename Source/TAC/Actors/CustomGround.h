// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAC/CustomCollisions/CustomColliders.h"
#include "TAC/CustomShapes/Components/CustomQuadComponent.h"

#include "CustomGround.generated.h"

UCLASS()
class TAC_API ACustomGround : public AActor, public ICustomHittable
{
	GENERATED_BODY()
	
public:	
	ACustomGround();

public:
	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();

public:
	/* ICustomRaycastHittable interface */
	virtual const FCustomBaseCollider& GetBoundCollider() const override { return QuadComponent->GetCollider(); }
	virtual const CustomCollidersArray& GetColliders() const override { return RaycastCollidersArray; }
	/***********************************/

private:
	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomQuadComponent* QuadComponent;

	CustomCollidersArray RaycastCollidersArray;
};
