// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomRaycastSystemContainer.h"

namespace CustomRaycastSystem {

	void Init(class UCustomRaycastSystemContainer* CustomRaycastSystemContainer);

	bool Raycast(const FVector& Origin, const FVector& Direction, struct FCustomLinecastResult& OutLinecastResult);

	struct FCustomLinecastResult
	{
	public:
		FCustomLinecastResult();
		void Reset();

	public:
		TWeakInterfacePtr<ICustomRaycastHittable> const GetHitActor() const { return HitActor; }
		FCustomRaycastBaseCollider* const GetHitCollider() const { return HitCollider; }
		const FVector& GetHitPoint() const { return HitPoint; }

	private:
		TWeakInterfacePtr<ICustomRaycastHittable> HitActor;
		FCustomRaycastBaseCollider* HitCollider;
		FVector HitPoint;

		friend bool Raycast(const FVector& Origin, const FVector& Direction, FCustomLinecastResult& OutLinecastResult);
	};
}
