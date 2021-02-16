// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameMode.h"
#include "TAC/CustomShapeTemplateDataAsset.h"
#include "TAC/TACPlayerController.h"
#include "TACCollisionSystemModule/Public/TACCollisionSystem.h"

ACustomGameMode::ACustomGameMode(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnObject(TEXT("Class'/Script/TAC.TACDefaultPawn'"));
	if (DefaultPawnObject.Class != NULL)
	{
		DefaultPawnClass = DefaultPawnObject.Class;
	}

	static ConstructorHelpers::FObjectFinder<UCustomShapeTemplateDataAsset> CustomShapeDataAsset(TEXT("CustomShapeTemplateDataAsset'/Game/TAC/CustomShapeTemplateDataAsset.CustomShapeTemplateDataAsset'"));
	if (CustomShapeDataAsset.Object != NULL)
	{
		CustomShapeTemplateData = CustomShapeDataAsset.Object;
	}

	static ConstructorHelpers::FClassFinder<ATACDefaultPlayerController> PlayerControllerObject(TEXT("Class'/Script/TAC.TACDefaultPlayerController'"));
	if (PlayerControllerObject.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerObject.Class;
	}
}