// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomRaycastSystemContainer.h"
#include "EngineUtils.h"

void UCustomRaycastSystemContainer::Init(UWorld* World)
{
	UE_LOG(LogTemp, Warning, TEXT("INIT"));

	HittableActors.Empty();

	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;

		ICustomRaycastHittable* HittableActor = Cast<ICustomRaycastHittable>(Actor);
		if (HittableActor == nullptr)
		{
			continue;
		}

		HittableActors.Add(HittableActor);
		Actor->OnDestroyed.AddDynamic(this, &UCustomRaycastSystemContainer::OnActorDestoryed);
	}

	World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UCustomRaycastSystemContainer::OnActorSpawned));
}


void UCustomRaycastSystemContainer::OnActorDestoryed(AActor* DestroyedActor)
{
	ICustomRaycastHittable* HittableActor = Cast<ICustomRaycastHittable>(DestroyedActor);

	if (HittableActor == nullptr)
	{
		return;
	}

	int32_t HittableActorIdx = HittableActors.Find(HittableActor);

	if (HittableActorIdx == INDEX_NONE)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ActorRemoved"));

	HittableActors.RemoveAt(HittableActorIdx);
}

void UCustomRaycastSystemContainer::OnActorSpawned(AActor* SpawnedActor)
{
	ICustomRaycastHittable* HittableActor = Cast<ICustomRaycastHittable>(SpawnedActor);

	if (HittableActor == nullptr)
	{
		return;
	}

	HittableActors.Add(HittableActor);
	SpawnedActor->OnDestroyed.AddDynamic(this, &UCustomRaycastSystemContainer::OnActorDestoryed);
}
