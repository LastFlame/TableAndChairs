// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameMode.h"
#include "TAC/CustomCollisions/CustomCollisionSystemContainer.h"
#include "TAC/CustomCollisions/CustomCollisionSystem.h"
#include "TAC/CustomShapeTemplateDataAsset.h"

ACustomGameMode::ACustomGameMode(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnObject(TEXT("Class'/Script/TAC.CustomPawn'"));
	if (DefaultPawnObject.Class != NULL)
	{
		DefaultPawnClass = DefaultPawnObject.Class;
	}

	static ConstructorHelpers::FObjectFinder<UCustomShapeTemplateDataAsset> CustomShapeDataAsset(TEXT("CustomShapeTemplateDataAsset'/Game/TAC/CustomShapeTemplateDataAsset.CustomShapeTemplateDataAsset'"));
	if (CustomShapeDataAsset.Object != NULL)
	{
		CustomShapeTemplateData = CustomShapeDataAsset.Object;
	}
}

void ACustomGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	CustomRaycastSystemContainer = NewObject<UCustomCollisionSystemContainer>();
	CustomRaycastSystemContainer->Init(GetWorld());
	CustomCollisionSystem::Init(CustomRaycastSystemContainer);
}
