// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomRaycastSystem.h"


namespace CustomRaycastSystem {

	static TWeakObjectPtr<UCustomRaycastSystemContainer> RaycastSystemContainer;

	void Init(UCustomRaycastSystemContainer* CustomRaycastSystemContainer)
	{
		RaycastSystemContainer = CustomRaycastSystemContainer;
	}

	bool Raycast(const FVector& Origin, const FVector& Direction, FCustomLinecastResult& OutLinecastResult)
	{
		OutLinecastResult.Reset();

		float PrevHitPointDistance = TNumericLimits<float>::Max();

		UE_LOG(LogTemp, Warning, TEXT("RaycastSystemContainer count %d"), RaycastSystemContainer->GetHittableActors().Num());
		for (ICustomRaycastHittable* HittableActor : RaycastSystemContainer->GetHittableActors())
		{
			FVector HitPoint;
			if (!HittableActor->GetBoundCollider()->HasBeenHit(Origin, Direction, HitPoint))
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

		PrevHitPointDistance = TNumericLimits<float>::Max();

		if (OutLinecastResult.HitActor != nullptr)
		{
			for (FCustomRaycastBaseCollider* RaycastCollider : OutLinecastResult.HitActor->GetColliders())
			{
				FVector HitPoint;
				if (!RaycastCollider->HasBeenHit(Origin, Direction, HitPoint))
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
		}

		return OutLinecastResult.HitActor != nullptr;
	}

	FCustomLinecastResult::FCustomLinecastResult() : HitActor(nullptr), HitCollider(nullptr), HitPoint(0.0f, 0.0f, 0.0f)
	{
	}

	void FCustomLinecastResult::Reset()
	{
		HitActor = nullptr;
		HitCollider = nullptr;
		HitPoint = { 0.0f, 0.0f, 0.0f };
	}
}
