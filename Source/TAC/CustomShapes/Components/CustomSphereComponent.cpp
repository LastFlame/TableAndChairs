// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomSphereComponent.h"
#include "TAC/CustomShapes/CustomShapesRenderer.h"

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

