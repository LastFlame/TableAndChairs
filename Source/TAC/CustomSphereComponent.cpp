// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomSphereComponent.h"
#include "CustomShapesRenderer.h"

void UCustomSphereComponent::BeginPlay()
{
	Super::BeginPlay();

	ICustomRaycastHittable* HittableOwner = Cast<ICustomRaycastHittable>(GetOwner());
	if (HittableOwner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s owner must implment ICustomRaycastHittable to receive collision events"), *GetName());
	}

	CustomSphereCollider.SetHittableActor(HittableOwner);
}

void UCustomSphereComponent::Draw()
{
	CustomShapeBuffers.Reset();

	CustomShapesRenderer::BeginScene(CustomShapeBuffers, *this);
	{
		CustomShapesRenderer::DrawSphere(CustomTransform);
	}
	CustomShapesRenderer::EndScene();
}

void UCustomSphereComponent::Draw(const FVector& Location, float Radius)
{
	CustomTransform.Location = Location;
	CustomTransform.Radius = Radius;

	Draw();
}

void UCustomSphereComponent::GenerateCollider()
{
	CustomSphereCollider.SetLocation(CustomTransform.Location);
	CustomSphereCollider.SetRadius(CustomTransform.Radius);
}

void UCustomSphereComponent::GenerateCollider(const FVector& Location, float Radius)
{
	CustomSphereCollider.SetLocation(Location);
	CustomSphereCollider.SetRadius(Radius);
}

