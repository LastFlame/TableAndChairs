// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CustomRaycastTypes.generated.h"

UINTERFACE(MinimalAPI)
class UCustomRaycastHittable : public UInterface
{
	GENERATED_BODY()
};

class TAC_API ICustomRaycastHittable
{
	GENERATED_BODY()

public:
	virtual TWeakObjectPtr<AActor> GetActor() const = 0;
	virtual const FCustomRaycastBaseCollider* GetBoundCollider() const = 0;
	virtual const TArray<FCustomRaycastBaseCollider*>& GetColliders() const = 0;

	virtual void OnHit(const FCustomRaycastBaseCollider* Collider, const FVector& HitPoint) = 0;
};

USTRUCT()
struct TAC_API FCustomRaycastBaseCollider
{
	GENERATED_USTRUCT_BODY()

	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const { return false; }

	const FVector& GetLocation() const { return Location; }
	void SetLocation(const FVector& NewLocation) { Location = NewLocation; }

	const ICustomRaycastHittable* GetHittableActor() const { return HittableActor; }
	ICustomRaycastHittable* GetHittableActor() { return HittableActor; }	
	void SetHittableActor(ICustomRaycastHittable* NewHittableActor) { HittableActor = NewHittableActor; }

protected:
	UPROPERTY(EditAnywhere)
	FVector Location;

	ICustomRaycastHittable* HittableActor = nullptr;
};

USTRUCT()
struct FCustomSphereRaycastCollider : public FCustomRaycastBaseCollider
{
	GENERATED_USTRUCT_BODY()

	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const override;
	
	float GetRadius() const { return Radius; }
	void SetRadius(float NewRadius) { Radius = NewRadius; }

protected:
	UPROPERTY(VisibleAnywhere)
	float Radius;
};

USTRUCT()
struct FCustomBoxRaycastCollider : public FCustomRaycastBaseCollider
{
	GENERATED_USTRUCT_BODY()

	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const override;

	const FVector& GetMinBounds() const { return MinBounds; }
	void SetMinBounds(const FVector& NewMinBounds) { MinBounds = NewMinBounds; }

	const FVector& GetMaxBounds() const { return MaxBounds; }
	void SetMaxBounds(const FVector& NewMaxBounds) { MaxBounds = NewMaxBounds; }

protected:
	UPROPERTY(EditAnywhere)
	FVector MinBounds;

	UPROPERTY(EditAnywhere)
	FVector MaxBounds;
};

USTRUCT()
struct FCustomPlaneRaycastCollider : public FCustomRaycastBaseCollider
{
	GENERATED_USTRUCT_BODY()

	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const override;

	const FVector& GetNormal() const { return Normal; }
	void SetNormal(const FVector& NewNormal) { Normal = NewNormal; }

protected:
	float PlaneEquation(const FVector& Origin, const FVector& Direction) const
	{
		return FVector::DotProduct(Normal, Origin) + Location.Size();
	}

protected:
	UPROPERTY(EditAnywhere)
	FVector Normal;
};