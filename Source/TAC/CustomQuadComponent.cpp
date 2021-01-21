// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomQuadComponent.h"
#include "CustomShapesRenderer.h"
#include "DrawDebugHelpers.h"

UCustomQuadComponent::UCustomQuadComponent(const FObjectInitializer& ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer)
{
	CustomTransform.Rotation.Y = 90.0f;
}

void UCustomQuadComponent::BeginPlay()
{
	Super::BeginPlay();

	ICustomRaycastHittable* HittableOwner = Cast<ICustomRaycastHittable>(GetOwner());
	if (HittableOwner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s owner must implment ICustomRaycastHittable to receive collision events"), *GetName());
	}

	CustomBoxCollider.SetHittableActor(HittableOwner);
}

void UCustomQuadComponent::Draw()
{
	CustomShapeBuffers.Reset();

	FCustomQuadMeshData Quad;
	CustomShapesRenderer::BeginScene(CustomShapeBuffers, *this);
	{
		Quad = CustomShapesRenderer::DrawQuad(CustomTransform);
	}
	CustomShapesRenderer::EndScene();
}

void UCustomQuadComponent::Draw(const FVector& Location, const FVector2D& Size)
{
	CustomTransform.Location = Location;
	CustomTransform.Size = Size;

	Draw();
}

void UCustomQuadComponent::GenerateCollider()
{
	CustomBoxCollider.SetMinBounds({ CustomTransform.Location.X - CustomTransform.Size.X, CustomTransform.Location.Y - CustomTransform.Size.Y, -1.f });
	CustomBoxCollider.SetMaxBounds({ CustomTransform.Location.X + CustomTransform.Size.X, CustomTransform.Location.Y + CustomTransform.Size.Y, 1.f });

	FlushPersistentDebugLines(GetWorld());
	DrawDebugSphere(GetWorld(), CustomBoxCollider.GetMinBounds(), 2.0f, 15.0f, FColor::Red, true);
	DrawDebugSphere(GetWorld(), CustomBoxCollider.GetMaxBounds(), 2.0f, 15.0f, FColor::Blue, true);
}

void UCustomQuadComponent::GenerateCollider(const FVector& Location, const FVector2D& Size)
{
	CustomBoxCollider.SetLocation(Location);
	CustomBoxCollider.SetMinBounds({ Location.X - Size.X, Location.Y - Size.Y, 0.1f });
	CustomBoxCollider.SetMaxBounds({ Location.X + Size.X, Location.Y + Size.Y, 0.1f });
}
