// Fill out your copyright notice in the Description page of Project Settings.


#include "TACSphereComponent.h"
#include "TACRenderSystemModule/Public/TACRenderSystem.h"

void UTACSphereComponent::Draw()
{
	CustomShapeBuffers.Reset();

	TACRender::BeginScene(CustomShapeBuffers, *this);
	{
		TACRender::DrawSphere(CustomTransform);
	}
	TACRender::EndScene();
}

void UTACSphereComponent::Draw(const FVector& Location, float Radius)
{
	CustomTransform.Location = Location;
	CustomTransform.Radius = Radius;

	Draw();
}

void UTACSphereComponent::GenerateCollider()
{
	CustomSphereCollider.SetLocation(CustomTransform.Location);
	CustomSphereCollider.SetRadius(CustomTransform.Radius);
}

void UTACSphereComponent::GenerateCollider(const FVector& Location, float Radius)
{
	CustomSphereCollider.SetLocation(Location);
	CustomSphereCollider.SetRadius(Radius);
}

