// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakInterfacePtr.h"
#include "UObject/NoExportTypes.h"
#include "CustomColliders.h"
#include "CustomCollisionSystemContainer.generated.h"


UCLASS()
class TAC_API UCustomCollisionSystemContainer : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(class UWorld* World);

public:
	const TArray<TWeakInterfacePtr<ICustomHittable>>& GetHittableActors() const { return HittableActors; }

private:
	UFUNCTION()
	void OnActorDestoryed(AActor* DestroyedActor);

	UFUNCTION()
	void OnActorSpawned(AActor* SpawnedActor);

private:
	TArray<TWeakInterfacePtr<ICustomHittable>> HittableActors;
};
