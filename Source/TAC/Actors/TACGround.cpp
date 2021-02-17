// Fill out your copyright notice in the Description page of Project Settings.


#include "TACGround.h"
#include "TAC/TACShapesTemplateData.h"

ATACGround::ATACGround() : RaycastCollidersArray(*this)
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

	QuadComponent = CreateDefaultSubobject<UTACQuadComponent>(TEXT("Plane"));
	QuadComponent->SetupAttachment(RootComponent);
	QuadComponent->GetCollider().SetFlag(ETACCollisionFlags::Static);

	static ConstructorHelpers::FObjectFinder<UTACShapesTemplateData> CustomShapeDataAsset(TEXT("TACShapesTemplateData'/Game/TAC/AssetData/TACShapesTemplateDataAsset.TACShapesTemplateDataAsset'"));
	if (CustomShapeDataAsset.Object != nullptr)
	{
		QuadComponent->GetCustomTransform().Size = CustomShapeDataAsset.Object->GetLocationBounds();
		Tags.Add(CustomShapeDataAsset.Object->GetTableSpawnerTag());
	}


	Generate();
}

void ATACGround::Generate()
{
	QuadComponent->Draw();
	QuadComponent->GenerateCollider();
}


