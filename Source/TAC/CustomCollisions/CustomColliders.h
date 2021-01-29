// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "CustomColliders.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FCustomOnLineTraceHitSignature, const FVector&);
DECLARE_MULTICAST_DELEGATE(FCustomOnLineTraceHitChangedSignature);

class CustomCollidersArray
{
public:
	CustomCollidersArray() {};
	CustomCollidersArray(class ICustomHittable& Owner);

public:
	void Add(struct FCustomBaseCollider& Element);
	void RemoveAt(int32_t Idx);

public:
	const TArray<struct FCustomBaseCollider*>& GetArray() const { return RaycastCollidersArray; }

protected:
	TWeakInterfacePtr<class ICustomHittable> Owner;
	TArray<struct FCustomBaseCollider*> RaycastCollidersArray;
};

UINTERFACE(MinimalAPI)
class UCustomHittable : public UInterface
{
	GENERATED_BODY()
};

class TAC_API ICustomHittable
{
	GENERATED_BODY()

public:
	virtual const FCustomBaseCollider& GetBoundCollider() const = 0;
	virtual const class CustomCollidersArray& GetColliders() const = 0;
};

USTRUCT()
struct TAC_API FCustomBaseCollider
{
	GENERATED_USTRUCT_BODY()

public:
	FCustomBaseCollider() {}
	virtual ~FCustomBaseCollider() { }

public:
	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const { return false; }
	virtual bool HasBeenHit(const struct FCustomBoxCollider& Box) const { return false; }

public:
	FCustomOnLineTraceHitSignature OnLineTraceHit;
	FCustomOnLineTraceHitChangedSignature OnLineTraceHitChanged;

	const FVector& GetLocation() const { return Location; }
	void SetLocation(const FVector& NewLocation) { Location = NewLocation; }

	const TWeakInterfacePtr<ICustomHittable> GetHittableActor() const { return HittableActor; }
	TWeakInterfacePtr<ICustomHittable> GetHittableActor() { return HittableActor; }

	void SetHittableActor(ICustomHittable& NewHittableActor) { HittableActor = NewHittableActor; }

protected:
	UPROPERTY(EditAnywhere)
	FVector Location;

	TWeakInterfacePtr<ICustomHittable> HittableActor;

	friend class CustomCollidersArray;
};

USTRUCT()
struct FCustomSphereCollider : public FCustomBaseCollider
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
struct FCustomBoxCollider : public FCustomBaseCollider
{
	GENERATED_USTRUCT_BODY()

	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const override;
	virtual bool HasBeenHit(const FCustomBoxCollider& Box) const override;

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
struct FCustomPlaneCollider : public FCustomBaseCollider
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