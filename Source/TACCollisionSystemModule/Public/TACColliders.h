// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/WeakInterfacePtr.h"
#include "TACColliders.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTACLineTraceHitSignature, const FVector&);
DECLARE_MULTICAST_DELEGATE(FOnTACLineTraceHitChangedSignature);

UENUM(Meta = (Bitflags))
enum class ETACCollisionFlags
{
	Ignore,
	Static,
	Dynamic
};

#define ENUM_TO_BIT(Enum) (1 << static_cast<uint8>(Enum))
#define COMPARE_ENUMS(EnumA, EnumB) ( (ENUM_TO_BIT(EnumA) & ENUM_TO_BIT(EnumB)) > 0) 

class TACCOLLISIONSYSTEMMODULE_API TACCollidersArray
{
public:
	TACCollidersArray() {};
	TACCollidersArray(class ITACHittable& Owner);

public:
	void Add(struct FTACBaseCollider& Element);
	void RemoveAt(int32_t Idx);

public:
	const TArray<struct FTACBaseCollider*>& GetArray() const { return RaycastCollidersArray; }

protected:
	TWeakInterfacePtr<class ITACHittable> Owner;
	TArray<struct FTACBaseCollider*> RaycastCollidersArray;
};

UINTERFACE(MinimalAPI)
class  UTACHittable : public UInterface
{
	GENERATED_BODY()
};

class TACCOLLISIONSYSTEMMODULE_API ITACHittable
{
	GENERATED_BODY()

public:
	virtual const FTACBaseCollider& GetBoundCollider() const = 0;
	virtual const class TACCollidersArray& GetColliders() const = 0;
};

USTRUCT()
struct TACCOLLISIONSYSTEMMODULE_API FTACBaseCollider
{
	GENERATED_USTRUCT_BODY()

public:
	FTACBaseCollider() : Location(0.0f, 0.0f, 0.0f), CollisionFlag(ETACCollisionFlags::Ignore) {}
	virtual ~FTACBaseCollider() { }

public:
	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const { return false; }
	virtual bool HasBeenHit(const struct FTACBoxCollider& Box) const { return false; }

public:
	FOnTACLineTraceHitSignature OnLineTraceHit;
	FOnTACLineTraceHitChangedSignature OnLineTraceHitChanged;

	const FVector& GetLocation() const { return Location; }
	void SetLocation(const FVector& NewLocation) { Location = NewLocation; }

	ETACCollisionFlags GetFlag() const { return CollisionFlag; }
	ETACCollisionFlags GetFlag() { return CollisionFlag; }
	void SetFlag(ETACCollisionFlags NewFlag) { CollisionFlag = NewFlag; }

	const TWeakInterfacePtr<ITACHittable> GetHittableActor() const { return HittableActor; }
	TWeakInterfacePtr<ITACHittable> GetHittableActor() { return HittableActor; }
	void SetHittableActor(ITACHittable& NewHittableActor) { HittableActor = NewHittableActor; }

protected:
	UPROPERTY(EditAnywhere)
	FVector Location;

	UPROPERTY(EditAnywhere)
	ETACCollisionFlags CollisionFlag;

	TWeakInterfacePtr<ITACHittable> HittableActor;

	friend class TACCollidersArray;
};

USTRUCT()
struct TACCOLLISIONSYSTEMMODULE_API FTACSphereCollider : public FTACBaseCollider
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
struct TACCOLLISIONSYSTEMMODULE_API FTACBoxCollider : public FTACBaseCollider
{
	GENERATED_USTRUCT_BODY()

	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const override;
	virtual bool HasBeenHit(const FTACBoxCollider& Box) const override;

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
struct TACCOLLISIONSYSTEMMODULE_API FTACPlaneCollider : public FTACBaseCollider
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