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

		ICustomRaycastHittable* CustomHittableActor = Cast<ICustomRaycastHittable>(Actor);
		if (CustomHittableActor == nullptr)
		{
			continue;
		}
		
		HittableActors.Add(*CustomHittableActor);
		Actor->OnDestroyed.AddDynamic(this, &UCustomRaycastSystemContainer::OnActorDestoryed);
	}

	World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UCustomRaycastSystemContainer::OnActorSpawned));
}


void UCustomRaycastSystemContainer::OnActorDestoryed(AActor* DestroyedActor)
{
	ICustomRaycastHittable* CustomHittableActor = Cast<ICustomRaycastHittable>(DestroyedActor);
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

void UCustomRaycastSystemContainer::OnActorSpawned(AActor* SpawnedActor)
{
	ICustomRaycastHittable* CustomHittableActor = Cast<ICustomRaycastHittable>(SpawnedActor);
	if (CustomHittableActor == nullptr)
	{
		return;
	}

	HittableActors.Add(*CustomHittableActor);
	SpawnedActor->OnDestroyed.AddDynamic(this, &UCustomRaycastSystemContainer::OnActorDestoryed);
}
