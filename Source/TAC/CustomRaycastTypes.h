// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DrawDebugHelpers.h"
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
	UPROPERTY(VisibleAnywhere)
	FVector Location;

	ICustomRaycastHittable* HittableActor = nullptr;
};

USTRUCT()
struct FCustomSphereRaycastCollider : public FCustomRaycastBaseCollider
{
	GENERATED_USTRUCT_BODY()

	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const override
	{
		const float DirectionSize = Direction.Size();

		const FVector FromSphereToOrigin = Origin - Location;
		const float FromSphereToOriginSize = FromSphereToOrigin.Size();

		const float a = DirectionSize * DirectionSize; // Same as Dot(dir,dir).
		const float b = 2 * FVector::DotProduct(Direction, FromSphereToOrigin);
		const float c = FromSphereToOriginSize * FromSphereToOriginSize - Radius * Radius;

		const float Discrim = b * b - 4 * a * c;

		bool bCollisionPointFound = false;

		if (Discrim > 0)
		{
			OutHitPoint = Origin + Direction * ((-b - sqrt(Discrim)) / (2 * a));
			bCollisionPointFound = true;
		}
		else if (Discrim == 0)
		{
			OutHitPoint = Origin + Direction * -b  / (2 * a);
			bCollisionPointFound = true;
		}

		if (bCollisionPointFound && HittableActor != nullptr)
		{
			HittableActor->OnHit(this, OutHitPoint);
		}

		return bCollisionPointFound;
	}

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

	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const override
	{
		float tmin = (MinBounds.Y - Origin.Y) / Direction.Y;
		float tmax = (MaxBounds.Y - Origin.Y) / Direction.Y;

		if (tmin > tmax)
		{
			float ttemp = tmax;

			tmax = tmin;
			tmin = ttemp;
		}

		float tzmin = (MinBounds.Z - Origin.Z) / Direction.Z;
		float tzmax = (MaxBounds.Z - Origin.Z) / Direction.Z;

		if (tzmin > tzmax)
		{
			float ttemp = tzmax;

			tzmax = tzmin;
			tzmin = ttemp;
		}

		if (tmin > tzmax || tzmin > tmax)
		{
			return false;
		}

		if (tzmin > tmin)
		{
			tmin = tzmin;
		}

		if (tzmax < tmax)
		{
			tmax = tzmax;
		}

		float txmin = (MinBounds.X - Origin.X) / Direction.X;
		float txmax = (MaxBounds.X - Origin.X) / Direction.X;

		if (txmin > txmax)
		{
			float ttemp = txmax;

			txmax = txmin;
			txmin = ttemp;
		}

		if (tmin > txmax || txmin > tmax)
		{
			return false;
		}

		if (tzmin > tmin)
		{
			tmin = tzmin;
		}

		if (tzmax < tmax)
		{
			tmax = tzmax;
		}

		OutHitPoint = Origin + Direction * tmin;

		if (HittableActor != nullptr)
		{
			HittableActor->OnHit(this, OutHitPoint);
		}

		return true;
	}

	const FVector& GetMinBounds() const { return MinBounds; }
	void SetMinBounds(const FVector& NewMinBounds) { MinBounds = NewMinBounds; }

	const FVector& GetMaxBounds() const { return MaxBounds; }
	void SetMaxBounds(const FVector& NewMaxBounds) { MaxBounds = NewMaxBounds; }

protected:
	UPROPERTY(VisibleAnywhere)
	FVector MinBounds;

	UPROPERTY(VisibleAnywhere)
	FVector MaxBounds;
};

USTRUCT()
struct FCustomPlaneRaycastCollider : public FCustomRaycastBaseCollider
{
	GENERATED_USTRUCT_BODY()

	virtual bool HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const override
	{
		constexpr float ParallelRayThreshold = 0.000015f;
		constexpr float PointOnPlaneThreshold = 0.000015f;

		float Denominator = FVector::DotProduct(Direction, Normal);
		if (abs(Denominator) < ParallelRayThreshold)
		{
			if (abs(PlaneEquation(Origin, Direction)) < PointOnPlaneThreshold)
			{
				OutHitPoint = Origin;

				if (HittableActor != nullptr)
				{
					HittableActor->OnHit(this, OutHitPoint);
				}

				return true;
			}

			return false;
		}

		float t = -PlaneEquation(Origin, Direction);
		t /= Denominator;

		if (t >= 0)
		{
			OutHitPoint = Origin + Direction * t;

			if (HittableActor != nullptr)
			{
				HittableActor->OnHit(this, OutHitPoint);
			}

			return true;
		}

		return false;
	}

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