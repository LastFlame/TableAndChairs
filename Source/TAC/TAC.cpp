// Fill out your copyright notice in the Description page of Project Settings.

#include "TAC.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, TAC, "TAC" );

bool IsLocationBetweenBounds(const FVector2D& LocationBounds, const FVector& MinLocation, const FVector& MaxLocation) 
{
	return !(MinLocation.Y < -LocationBounds.Y || MinLocation.X < -LocationBounds.X
		|| MaxLocation.Y > LocationBounds.Y || MaxLocation.X > LocationBounds.X);
}
