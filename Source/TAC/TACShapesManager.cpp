// Fill out your copyright notice in the Description page of Project Settings.


#include "TACShapesManager.h"
#include "TAC/Shapes/TACTableShape.h"
#include "TACCollisionSystemModule/Public/TACCollisionSystem.h"

UTACShapesManager::UTACShapesManager() : UWorldSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UTACShapesTemplateData> TableTemplateAsset(TEXT("TACShapesTemplateData'/Game/TAC/AssetData/TACShapesTemplateDataAsset.TACShapesTemplateDataAsset'"));
	if (TableTemplateAsset.Object != NULL)
	{
		ShapesTemplateData = TableTemplateAsset.Object;
	}
}

ATACTableShape* UTACShapesManager::CreateTable(const FVector& Location) const
{
	if (ShapesTemplateData == nullptr)
	{
		return nullptr;
	}

	const FTACBoxCollider& TableDefaultBounds = ShapesTemplateData->GetDefaultTableBoundCollider();

	FTACBoxCollider TableToSpawnCollider;
	TableToSpawnCollider.SetMinBounds(Location + TableDefaultBounds.GetMinBounds());
	TableToSpawnCollider.SetMaxBounds(Location + TableDefaultBounds.GetMaxBounds());

	if (!IsLocationBetweenBounds(ShapesTemplateData->GetLocationBounds(), TableToSpawnCollider.GetMinBounds(), TableToSpawnCollider.GetMaxBounds()))
	{
		return nullptr;
	}

	FTACCollisionResult Result;
	if (GetWorld()->GetSubsystem<UTACCollisionSystem>()->BoxTrace(TableToSpawnCollider, ETACCollisionFlags::Static, Result))
	{
		return nullptr;
	}

	ATACTableShape* CustomShape = GetWorld()->SpawnActor<ATACTableShape>(FVector::ZeroVector, FRotator::ZeroRotator);
	CustomShape->SetCustomLocation(Location.X, Location.Y);
	CustomShape->Generate();

	return CustomShape;
}

void UTACShapesManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}
