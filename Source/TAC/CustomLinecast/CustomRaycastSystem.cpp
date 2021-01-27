// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomRaycastSystem.h"


namespace CustomRaycastSystem {

	static TWeakObjectPtr<UCustomRaycastSystemContainer> RaycastSystemContainer;

	static TWeakInterfacePtr<ICustomRaycastHittable> PrevHitActor;
	static FCustomRaycastBaseCollider* PrevHitCollider;

	void Init(UCustomRaycastSystemContainer* CustomRaycastSystemContainer)
	{
		RaycastSystemContainer = CustomRaycastSystemContainer;
		PrevHitActor = TWeakInterfacePtr<ICustomRaycastHittable>();
		PrevHitCollider = nullptr;
	}

	bool Raycast(const FVector& Origin, const FVector& Direction, FCustomLinecastResult& OutLinecastResult)
	{
		OutLinecastResult.Reset();

		float PrevHitPointDistance = TNumericLimits<float>::Max();

		for (TWeakInterfacePtr<ICustomRaycastHittable> HittableActor : RaycastSystemContainer->GetHittableActors())
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
				PrevHitActor->GetBoundCollider().OnHitChanged.Broadcast();
			}

			OutLinecastResult.HitActor->GetBoundCollider().OnHit.Broadcast(OutLinecastResult.HitPoint);
			PrevHitActor = OutLinecastResult.HitActor;

			for (FCustomRaycastBaseCollider* RaycastCollider : OutLinecastResult.HitActor->GetColliders().GetArray())
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
				PrevHitCollider->OnHitChanged.Broadcast();
			}

			if (OutLinecastResult.HitCollider != nullptr)
			{
				OutLinecastResult.HitCollider->OnHit.Broadcast(OutLinecastResult.HitPoint);
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
		HitActor = TWeakInterfacePtr<ICustomRaycastHittable>();
		HitCollider = nullptr;
		HitPoint = { 0.0f, 0.0f, 0.0f };
	}
}
