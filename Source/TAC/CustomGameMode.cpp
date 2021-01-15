// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameMode.h"
#include "CustomRaycastSystemContainer.h"
#include "CustomRaycastSystem.h"

ACustomGameMode::ACustomGameMode(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnObject(TEXT("Class'/Script/TAC.CustomPawn'"));
	if (DefaultPawnObject.Class != NULL)
	{
		DefaultPawnClass = DefaultPawnObject.Class;
	}	
}

void ACustomGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	CustomRaycastSystemContainer = NewObject<UCustomRaycastSystemContainer>();
	CustomRaycastSystemContainer->Init(GetWorld());
	CustomRaycastSystem::Init(CustomRaycastSystemContainer);

}
