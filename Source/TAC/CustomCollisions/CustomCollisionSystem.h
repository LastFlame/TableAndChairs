// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomCollisionSystemContainer.h"

namespace CustomCollisionSystem {

	void Init(class UCustomCollisionSystemContainer* CustomRaycastSystemContainer);

	bool LineTrace(const FVector& Origin, const FVector& Direction, struct FCustomCollisionResult& OutCollisionResult);

	bool BoxTrace(const FCustomBoxCollider& Box, ECustomCollisionFlags CollidersToSkip, struct FCustomCollisionResult& OutCollisionResult);

	struct FCustomCollisionResult
	{
	public:
		FCustomCollisionResult();

	public:
		void Reset();

	public:
		TWeakInterfacePtr<ICustomHittable> const GetHitActor() const { return HitActor; }
		FCustomBaseCollider* const GetHitCollider() const { return HitCollider; }
		const FVector& GetHitPoint() const { return HitPoint; }

	private:
		TWeakInterfacePtr<ICustomHittable> HitActor;
		FCustomBaseCollider* HitCollider;
		FVector HitPoint;

		friend bool LineTrace(const FVector& Origin, const FVector& Direction, FCustomCollisionResult& OutCollisionResult);
		friend bool BoxTrace(const FCustomBoxCollider& Box, ECustomCollisionFlags CollidersToSkip, FCustomCollisionResult& OutCollisionResult);
	};
}
