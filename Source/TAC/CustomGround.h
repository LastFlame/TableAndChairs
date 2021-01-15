// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomRaycastTypes.h"
#include "CustomQuadComponent.h"

#include "CustomGround.generated.h"

UCLASS()
class TAC_API ACustomGround : public AActor, public ICustomRaycastHittable
{
	GENERATED_BODY()
	
public:	
	ACustomGround();

public:
	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();

public:
	virtual void OnHit(const FCustomRaycastBaseCollider* Collider, const FVector& HitPoint) override {};
	virtual TWeakObjectPtr<AActor> GetActor() const override { return (AActor*)this; }
	virtual const FCustomRaycastBaseCollider* GetBoundCollider() const override { return QuadComponent->GetCollider(); }
	virtual const TArray<FCustomRaycastBaseCollider*>& GetColliders() const override { return RaycastColliders; }

private:
	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
	UCustomQuadComponent* QuadComponent;

	TArray<FCustomRaycastBaseCollider*> RaycastColliders;
};
