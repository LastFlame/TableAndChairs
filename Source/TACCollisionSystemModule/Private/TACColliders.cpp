#include "TACCollisionSystemModule/Public/TACColliders.h"

TACCollidersArray::TACCollidersArray(ITACHittable& Owner) : Owner(Owner) {}

void TACCollidersArray::Add(FTACBaseCollider& Element)
{
	Element.HittableActor = Owner;
	RaycastCollidersArray.Add(&Element);
}

void TACCollidersArray::RemoveAt(int32_t Idx)
{
	RaycastCollidersArray[Idx]->HittableActor = TWeakInterfacePtr<ITACHittable>();
	RaycastCollidersArray.RemoveAt(Idx);
}

bool FTACSphereCollider::HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const
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
		OutHitPoint = Origin + Direction * -b / (2 * a);
		bCollisionPointFound = true;
	}

	return bCollisionPointFound;
}


bool FTACBoxCollider::HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const
{
	float TNear = -TNumericLimits<float>::Max();
	float TFar = TNumericLimits<float>::Max();

	constexpr float ParallelDirThreshold = 0.00000015f;

	for (int i = 0; i < 3; ++i)
	{
		if (abs(Direction[i]) <= ParallelDirThreshold) // Is ray parallel to  planes in current direction
		{
			if (Origin[i] < MinBounds[i] || Origin[i] > MaxBounds[i]) 
			{
				return false; // Parallel and outside the box no intersections
			}
		}

		// Ray not parallel, find intersections
		float T0 = (MinBounds[i] - Origin[i]) / Direction[i];
		float T1 = (MaxBounds[i] - Origin[i]) / Direction[i];

		// Check T0 T1 order
		if (T0 > T1)
		{
			float Tmp = T1;
			T1 = T0;
			T0 = Tmp;
		}

		if (T0 > TNear)
		{
			TNear = T0;
		}

		if (T1 < TFar)
		{
			TFar = T1;
		}

		// Check ray miss
		if (TNear > TFar || TFar < 0.0f)
		{
			return false; 
		}
	}

	// If the code has reach this point, there's an intersection
	OutHitPoint = Origin + Direction * (TNear > 0.0f ? TNear : TFar);

	return true;
}


bool FTACBoxCollider::HasBeenHit(const FTACBoxCollider& Box) const
{
	for (int i = 0; i < 3; ++i)
	{
		if (MinBounds[i] > Box.MaxBounds[i] || Box.MinBounds[i] > MaxBounds[i])
		{
			return false;
		}
	}

	return true;
}

bool FTACPlaneCollider::HasBeenHit(const FVector& Origin, const FVector& Direction, FVector& OutHitPoint) const
{
	constexpr float ParallelRayThreshold = 0.000015f;
	constexpr float PointOnPlaneThreshold = 0.000015f;

	float Denominator = FVector::DotProduct(Direction, Normal);
	if (abs(Denominator) < ParallelRayThreshold)
	{
		if (abs(PlaneEquation(Origin, Direction)) < PointOnPlaneThreshold)
		{
			OutHitPoint = Origin;
			return true;
		}

		return false;
	}

	float t = -PlaneEquation(Origin, Direction);
	t /= Denominator;

	if (t >= 0)
	{
		OutHitPoint = Origin + Direction * t;
		return true;
	}

	return false;
}
