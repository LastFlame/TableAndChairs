// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TAC/Shapes/TACQuadComponent.h"
#include "TACCollisionSystemModule/Public/TACColliders.h"
#include "TACGround.generated.h"

UCLASS()
class TAC_API ATACGround : public AActor, public ITACHittable
{
	GENERATED_BODY()
	
public:	
	ATACGround();

public:
	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();

public:
	/* ICustomRaycastHittable interface */
	virtual TWeakObjectPtr<AActor> GetActor() const  override { return (AActor*)this; }
	virtual const FTACBaseCollider& GetBoundCollider() const override { return QuadComponent->GetCollider(); }
	virtual const TACCollidersArray& GetColliders() const override { return RaycastCollidersArray; }
	/***********************************/

private:
	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UTACQuadComponent* QuadComponent;

	TACCollidersArray RaycastCollidersArray;
};
