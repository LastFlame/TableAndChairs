// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCollisionSystem.h"


namespace CustomCollisionSystem {

	static TWeakObjectPtr<UCustomCollisionSystemContainer> RaycastSystemContainer;

	static TWeakInterfacePtr<ICustomHittable> PrevHitActor;
	static FCustomBaseCollider* PrevHitCollider;

	void Init(UCustomCollisionSystemContainer* CustomRaycastSystemContainer)
	{
		RaycastSystemContainer = CustomRaycastSystemContainer;
		PrevHitActor = TWeakInterfacePtr<ICustomHittable>();
		PrevHitCollider = nullptr;
	}

	bool LineTrace(const FVector& Origin, const FVector& Direction, FCustomCollisionResult& OutCollisionResult)
	{
		OutCollisionResult.Reset();

		float PrevHitPointDistance = TNumericLimits<float>::Max();

		for (TWeakInterfacePtr<ICustomHittable> HittableActor : RaycastSystemContainer->GetHittableActors())
		{
			if (!HittableActor.IsValid())
			{
				continue;
			}

			FVector HitPoint;
			if (!HittableActor->GetBoundCollider().HasBeenHit(Origin, Direction, HitPoint))
			{
				continue;
			}

			const float Distance = FVector::Distance(Origin, HitPoint);
			if (Distance < PrevHitPointDistance)
			{
				OutCollisionResult.HitActor = HittableActor;
				PrevHitPointDistance = Distance;
				OutCollisionResult.HitPoint = HitPoint;
			}
		}

		bool bHitActorIsValid = OutCollisionResult.HitActor.IsValid();

		if (bHitActorIsValid)
		{
			PrevHitPointDistance = TNumericLimits<float>::Max();

			if (PrevHitActor.IsValid())
			{
				PrevHitActor->GetBoundCollider().OnLineTraceHitChanged.Broadcast();
			}

			OutCollisionResult.HitActor->GetBoundCollider().OnLineTraceHit.Broadcast(OutCollisionResult.HitPoint);
			PrevHitActor = OutCollisionResult.HitActor;

			for (FCustomBaseCollider* Collider : OutCollisionResult.HitActor->GetColliders().GetArray())
			{
				FVector HitPoint;
				if (Collider != nullptr && !Collider->HasBeenHit(Origin, Direction, HitPoint))
				{
					continue;
				}

				const float Distance = FVector::Distance(Origin, HitPoint);
				if (Distance < PrevHitPointDistance)
				{
					OutCollisionResult.HitCollider = Collider;
					PrevHitPointDistance = Distance;
					OutCollisionResult.HitPoint = HitPoint;
				}
			}

			if (PrevHitCollider != nullptr && PrevHitCollider->GetHittableActor().IsValid())
			{
				PrevHitCollider->OnLineTraceHitChanged.Broadcast();
			}

			if (OutCollisionResult.HitCollider != nullptr)
			{
				OutCollisionResult.HitCollider->OnLineTraceHit.Broadcast(OutCollisionResult.HitPoint);
			}

			PrevHitCollider = OutCollisionResult.HitCollider;
		}

		return bHitActorIsValid;
	}

	bool BoxTrace(const FCustomBoxCollider& Box, ECustomCollisionFlags CollidersToSkip, FCustomCollisionResult& OutCollisionResult)
	{
		OutCollisionResult.Reset();

		float PrevHitPointDistance = TNumericLimits<float>::Max();

		for (TWeakInterfacePtr<ICustomHittable> HittableActor : RaycastSystemContainer->GetHittableActors())
		{
			if (!HittableActor.IsValid())
			{
				continue;
			}

			if (COMPARE_ENUMS(CollidersToSkip, HittableActor->GetBoundCollider().GetFlag()))
			{
				continue;
			}

			if (!HittableActor->GetBoundCollider().HasBeenHit(Box))
			{
				continue;
			}

			const float Distance = FVector::Distance(Box.GetLocation(), HittableActor->GetBoundCollider().GetLocation());
			if (Distance < PrevHitPointDistance)
			{
				OutCollisionResult.HitActor = HittableActor;
				PrevHitPointDistance = Distance;
			}
		}

		bool bHitActorIsValid = OutCollisionResult.HitActor.IsValid();

		if (bHitActorIsValid)
		{
			PrevHitPointDistance = TNumericLimits<float>::Max();

			for (FCustomBaseCollider* Collider : OutCollisionResult.HitActor->GetColliders().GetArray())
			{
				if (Collider == nullptr)
				{
					continue;
				}

				if (COMPARE_ENUMS(CollidersToSkip, Collider->GetFlag()))
				{
					continue;
				}

				if (!Collider->HasBeenHit(Box))
				{
					continue;
				}

				const float Distance = FVector::Distance(Box.GetLocation(), Collider->GetLocation());
				if (Distance < PrevHitPointDistance)
				{
					OutCollisionResult.HitCollider = Collider;
					PrevHitPointDistance = Distance;
				}
			}
		}

		return bHitActorIsValid;
	}

	FCustomCollisionResult::FCustomCollisionResult() : HitActor(), HitCollider(nullptr), HitPoint(0.0f, 0.0f, 0.0f)
	{
	}

	void FCustomCollisionResult::Reset()
	{
		HitActor = TWeakInterfacePtr<ICustomHittable>();
		HitCollider = nullptr;
		HitPoint = { 0.0f, 0.0f, 0.0f };
	}
}
