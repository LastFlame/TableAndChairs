// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomCollisionSystemContainer.h"

namespace CustomLinecastSystem {

	void Init(class UCustomCollisionSystemContainer* CustomRaycastSystemContainer);

	bool Linecast(const FVector& Origin, const FVector& Direction, struct FCustomLinecastResult& OutLinecastResult);

	struct FCustomLinecastResult
	{
	public:
		FCustomLinecastResult();
		void Reset();

	public:
		TWeakInterfacePtr<ICustomHittable> const GetHitActor() const { return HitActor; }
		FCustomBaseCollider* const GetHitCollider() const { return HitCollider; }
		const FVector& GetHitPoint() const { return HitPoint; }

	private:
		TWeakInterfacePtr<ICustomHittable> HitActor;
		FCustomBaseCollider* HitCollider;
		FVector HitPoint;

		friend bool Linecast(const FVector& Origin, const FVector& Direction, FCustomLinecastResult& OutLinecastResult);
	};
}
