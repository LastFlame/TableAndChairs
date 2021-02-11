// Fill out your copyright notice in the Description page of Project Settings.


#include "TACCollisionSystemModule/Public/TACCollisionSystem.h"
#include "EngineUtils.h"

static TWeakInterfacePtr<ITACHittable> PrevHitActor;
static FTACBaseCollider* PrevHitCollider;

void UTACCollisionSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("TAC COLLISION SYSTEM INITIALIZED"));

	OnWorldBeginHandler = GetWorld()->OnWorldBeginPlay.AddUObject(this, &UTACCollisionSystem::OnWorldBegin);
}

void UTACCollisionSystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("TAC COLLISION SYSTEM DEINITIALIZED"));
	HittableActors.Empty();
	GetWorld()->OnWorldBeginPlay.Remove(OnWorldBeginHandler);
}

bool UTACCollisionSystem::LineTrace(const FVector& Origin, const FVector& Direction, FTACCollisionResult& OutCollisionResult)
{
	OutCollisionResult.Reset();

	float PrevHitPointDistance = TNumericLimits<float>::Max();

	for (TWeakInterfacePtr<ITACHittable> HittableActor : HittableActors)
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

		for (FTACBaseCollider* Collider : OutCollisionResult.HitActor->GetColliders().GetArray())
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

bool UTACCollisionSystem::BoxTrace(const FTACBoxCollider& Box, ETACCollisionFlags CollidersToSkip, FTACCollisionResult& OutCollisionResult)
{
	OutCollisionResult.Reset();

	float PrevHitPointDistance = TNumericLimits<float>::Max();

	for (TWeakInterfacePtr<ITACHittable> HittableActor : HittableActors)
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

		for (FTACBaseCollider* Collider : OutCollisionResult.HitActor->GetColliders().GetArray())
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

void UTACCollisionSystem::OnActorDestoryed(AActor* DestroyedActor)
{
	ITACHittable* CustomHittableActor = Cast<ITACHittable>(DestroyedActor);
	if (CustomHittableActor == nullptr)
	{
		return;
	}

	int32_t HittableActorIdx = HittableActors.Find(*CustomHittableActor);

	if (HittableActorIdx == INDEX_NONE)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ActorRemoved"));

	HittableActors.RemoveAt(HittableActorIdx);
}

void UTACCollisionSystem::OnActorSpawned(AActor* SpawnedActor)
{
	ITACHittable* CustomHittableActor = Cast<ITACHittable>(SpawnedActor);
	if (CustomHittableActor == nullptr)
	{
		return;
	}

	HittableActors.Add(*CustomHittableActor);
	SpawnedActor->OnDestroyed.AddDynamic(this, &UTACCollisionSystem::OnActorDestoryed);
}

void UTACCollisionSystem::OnWorldBegin()
{
	UE_LOG(LogTemp, Warning, TEXT("ONWORLDBEGIN"));

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;

		ITACHittable* CustomHittableActor = Cast<ITACHittable>(Actor);
		if (CustomHittableActor == nullptr)
		{
			continue;
		}

		HittableActors.Add(*CustomHittableActor);
		Actor->OnDestroyed.AddDynamic(this, &UTACCollisionSystem::OnActorDestoryed);
	}

	GetWorld()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UTACCollisionSystem::OnActorSpawned));
}
