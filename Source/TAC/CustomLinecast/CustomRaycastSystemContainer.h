// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakInterfacePtr.h"
#include "UObject/NoExportTypes.h"
#include "CustomRaycastTypes.h"
#include "CustomRaycastSystemContainer.generated.h"


UCLASS()
class TAC_API UCustomRaycastSystemContainer : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(class UWorld* World);

public:
	const TArray<TWeakInterfacePtr<ICustomRaycastHittable>>& GetHittableActors() const { return HittableActors; }

private:
	UFUNCTION()
	void OnActorDestoryed(AActor* DestroyedActor);

	UFUNCTION()
	void OnActorSpawned(AActor* SpawnedActor);

private:
	TArray<TWeakInterfacePtr<ICustomRaycastHittable>> HittableActors;
};
