// Fill out your copyright notice in the Description page of Project Settings.


#include "TACGameMode.h"
#include "TAC/TACPlayerController.h"

ATACGameMode::ATACGameMode(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnObject(TEXT("Class'/Script/TAC.TACDefaultPawn'"));
	if (DefaultPawnObject.Class != NULL)
	{
		DefaultPawnClass = DefaultPawnObject.Class;
	}

	static ConstructorHelpers::FClassFinder<ATACDefaultPlayerController> PlayerControllerObject(TEXT("Class'/Script/TAC.TACDefaultPlayerController'"));
	if (PlayerControllerObject.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerObject.Class;
	}
}