// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "CustomRaycastTypes.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FCustomLinecastColliderHitSignature, const FVector&);
DECLARE_MULTICAST_DELEGATE(FCustomLinecastColliderHitChangedSignature);


class CustomRaycastCollidersArray
{
public:
	CustomRaycastCollidersArray() {};
	CustomRaycastCollidersArray(class ICustomRaycastHittable& Owner);

public:
	void Add(struct FCustomRaycastBaseCollider& Element);
	void RemoveAt(int32_t Idx);

public:
	const TArray<struct FCustomRaycastBaseCollider*>& GetArray() const { return RaycastCollidersArray; }

protected:
	TWeakInterfacePtr<class ICustomRaycastHittable> Owner;
	TArray<struct FCustomRaycastBaseCollider*> RaycastCollidersArray;
};

UINTERFACE(MinimalAPI)
class UCustomRaycastHittable : public UInterface
{
	GENERATED_BODY()
};

class TAC_API ICustomRaycastHittable
{
	GENERATED_BODY()

public:
	virtual const FCustomRaycastBaseCollider& GetBoundCollider() const = 0;
	virtual const class CustomRaycastCollidersArray& GetColliders() const = 0;
};

USTRUCT()
struct TAC_API FCustomRaycastBaseCollider
{
	GENERATED_USTRUCT_BODY()

	FCustomRaycastBaseCollider() {}
	virtual ~FCustomRaycastBaseCollider() { }

public:
	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const { return false; }

public:
	FCustomLinecastColliderHitSignature OnHit;	
	FCustomLinecastColliderHitChangedSignature OnHitChanged;

	const FVector& GetLocation() const { return Location; }
	void SetLocation(const FVector& NewLocation) { Location = NewLocation; }

	const TWeakInterfacePtr<ICustomRaycastHittable> GetHittableActor() const { return HittableActor; }
	TWeakInterfacePtr<ICustomRaycastHittable> GetHittableActor() { return HittableActor; }

	void SetHittableActor(ICustomRaycastHittable& NewHittableActor) { HittableActor = NewHittableActor; }

protected:
	UPROPERTY(EditAnywhere)
	FVector Location;

	TWeakInterfacePtr<ICustomRaycastHittable> HittableActor;

	friend class CustomRaycastCollidersArray;
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