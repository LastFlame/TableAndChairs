// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGround.h"

ACustomGround::ACustomGround() : RaycastCollidersArray(*this)
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

	QuadComponent = CreateDefaultSubobject<UCustomQuadComponent>(TEXT("Plane"));
	QuadComponent->SetupAttachment(RootComponent);
}

void ACustomGround::Generate()
{
	QuadComponent->Draw();
	QuadComponent->GenerateCollider();
}


