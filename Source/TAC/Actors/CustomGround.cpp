// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGround.h"
#include "TAC/CustomShapeTemplateDataAsset.h"

ACustomGround::ACustomGround() : RaycastCollidersArray(*this)
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

	QuadComponent = CreateDefaultSubobject<UCustomQuadComponent>(TEXT("Plane"));
	QuadComponent->SetupAttachment(RootComponent);
	QuadComponent->GetCollider().SetFlag(ETACCollisionFlags::Static);

	static ConstructorHelpers::FObjectFinder<UCustomShapeTemplateDataAsset> CustomShapeDataAsset(TEXT("CustomShapeTemplateDataAsset'/Game/TAC/CustomShapeTemplateDataAsset.CustomShapeTemplateDataAsset'"));
	if (CustomShapeDataAsset.Object != nullptr)
	{
		QuadComponent->GetCustomTransform().Size = CustomShapeDataAsset.Object->GetLocationBounds();
	}

	Generate();
}

void ACustomGround::Generate()
{
	QuadComponent->Draw();
	QuadComponent->GenerateCollider();
}


