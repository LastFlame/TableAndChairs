// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakInterfacePtr.h"
#include "Subsystems/WorldSubsystem.h"
#include "TACCollisionSystemModule/Public/TACColliders.h"
#include "TACCollisionSystem.generated.h"

struct TACCOLLISIONSYSTEMMODULE_API FTACCollisionResult
{
public:
	FTACCollisionResult() : HitCollider(nullptr), HitPoint(0.0f, 0.0f, 0.0f)
	{
	}

public:
	void Reset()
	{
		HitActor = TWeakInterfacePtr<ITACHittable>();
		HitCollider = nullptr;
		HitPoint = { 0.0f, 0.0f, 0.0f };
	}

public:
	TWeakInterfacePtr<ITACHittable> const GetHitActor() const { return HitActor; }
	FTACBaseCollider* const GetHitCollider() const { return HitCollider; }
	const FVector& GetHitPoint() const { return HitPoint; }

private:
	TWeakInterfacePtr<ITACHittable> HitActor;
	FTACBaseCollider* HitCollider;
	FVector HitPoint;

private:
	friend class UTACCollisionSystem;
};

UCLASS()
class TACCOLLISIONSYSTEMMODULE_API UTACCollisionSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool LineTrace(const FVector& Origin, const FVector& Direction, struct FTACCollisionResult& OutCollisionResult);
	bool BoxTrace(const FTACBoxCollider& Box, ETACCollisionFlags CollidersToSkip, struct FTACCollisionResult& OutCollisionResult);

public:
	const TArray<TWeakInterfacePtr<ITACHittable>>& GetHittableActors() const { return HittableActors; }

private:
	UFUNCTION()
	void OnActorDestoryed(AActor* DestroyedActor);

	UFUNCTION()
	void OnActorSpawned(AActor* SpawnedActor);

	UFUNCTION()
	void OnWorldBegin();

	FDelegateHandle OnWorldBeginHandler;

private:
	TArray<TWeakInterfacePtr<ITACHittable>> HittableActors;
};


