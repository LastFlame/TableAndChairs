// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCollisionSystemContainer.h"
#include "EngineUtils.h"

void UCustomCollisionSystemContainer::Init(UWorld* World)
{
	UE_LOG(LogTemp, Warning, TEXT("INIT"));

	HittableActors.Empty();

	for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;

		ICustomHittable* CustomHittableActor = Cast<ICustomHittable>(Actor);
		if (CustomHittableActor == nullptr)
		{
			continue;
		}
		
		HittableActors.Add(*CustomHittableActor);
		Actor->OnDestroyed.AddDynamic(this, &UCustomCollisionSystemContainer::OnActorDestoryed);
	}

	World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UCustomCollisionSystemContainer::OnActorSpawned));
}


void UCustomCollisionSystemContainer::OnActorDestoryed(AActor* DestroyedActor)
{
	ICustomHittable* CustomHittableActor = Cast<ICustomHittable>(DestroyedActor);
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

void UCustomCollisionSystemContainer::OnActorSpawned(AActor* SpawnedActor)
{
	ICustomHittable* CustomHittableActor = Cast<ICustomHittable>(SpawnedActor);
	if (CustomHittableActor == nullptr)
	{
		return;
	}

	HittableActors.Add(*CustomHittableActor);
	SpawnedActor->OnDestroyed.AddDynamic(this, &UCustomCollisionSystemContainer::OnActorDestoryed);
}
