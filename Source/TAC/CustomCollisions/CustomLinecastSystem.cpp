// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomLinecastSystem.h"


namespace CustomLinecastSystem {

	static TWeakObjectPtr<UCustomCollisionSystemContainer> RaycastSystemContainer;

	static TWeakInterfacePtr<ICustomHittable> PrevHitActor;
	static FCustomBaseCollider* PrevHitCollider;

	void Init(UCustomCollisionSystemContainer* CustomRaycastSystemContainer)
	{
		RaycastSystemContainer = CustomRaycastSystemContainer;
		PrevHitActor = TWeakInterfacePtr<ICustomHittable>();
		PrevHitCollider = nullptr;
	}

	bool Linecast(const FVector& Origin, const FVector& Direction, FCustomLinecastResult& OutLinecastResult)
	{
		OutLinecastResult.Reset();

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
				OutLinecastResult.HitActor = HittableActor;
				PrevHitPointDistance = Distance;
				OutLinecastResult.HitPoint = HitPoint;
			}
		}

		if (OutLinecastResult.HitActor.IsValid())
		{
			PrevHitPointDistance = TNumericLimits<float>::Max();

			if (PrevHitActor.IsValid())
			{
				PrevHitActor->GetBoundCollider().OnCollisionExit.Broadcast();
			}

			OutLinecastResult.HitActor->GetBoundCollider().OnCollisionEnter.Broadcast(OutLinecastResult.HitPoint);
			PrevHitActor = OutLinecastResult.HitActor;

			for (FCustomBaseCollider* RaycastCollider : OutLinecastResult.HitActor->GetColliders().GetArray())
			{
				FVector HitPoint;
				if (RaycastCollider != nullptr && !RaycastCollider->HasBeenHit(Origin, Direction, HitPoint))
				{
					continue;
				}

				const float Distance = FVector::Distance(Origin, HitPoint);
				if (Distance < PrevHitPointDistance)
				{
					OutLinecastResult.HitCollider = RaycastCollider;
					PrevHitPointDistance = Distance;
					OutLinecastResult.HitPoint = HitPoint;
				}
			}

			if (PrevHitCollider != nullptr && PrevHitCollider->GetHittableActor().IsValid())
			{
				PrevHitCollider->OnCollisionExit.Broadcast();
			}

			if (OutLinecastResult.HitCollider != nullptr)
			{
				OutLinecastResult.HitCollider->OnCollisionEnter.Broadcast(OutLinecastResult.HitPoint);
			}

			PrevHitCollider = OutLinecastResult.HitCollider;
		}

		return OutLinecastResult.HitActor.IsValid();
	}

	FCustomLinecastResult::FCustomLinecastResult() : HitActor(), HitCollider(nullptr), HitPoint(0.0f, 0.0f, 0.0f)
	{
	}

	void FCustomLinecastResult::Reset()
	{
		HitActor = TWeakInterfacePtr<ICustomHittable>();
		HitCollider = nullptr;
		HitPoint = { 0.0f, 0.0f, 0.0f };
	}
}
