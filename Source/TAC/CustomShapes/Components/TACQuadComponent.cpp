// Fill out your copyright notice in the Description page of Project Settings.


#include "TACQuadComponent.h"
#include "TACRenderSystemModule/Public/TACRenderSystem.h"
#include "DrawDebugHelpers.h"

UTACQuadComponent::UTACQuadComponent(const FObjectInitializer& ObjectInitializer) : UProceduralMeshComponent(ObjectInitializer)
{
	CustomTransform.Rotation.Y = 90.0f;
}

void UTACQuadComponent::Draw()
{
	CustomShapeBuffers.Reset();

	FTACQuadMeshData Quad;
	TACRender::BeginScene(CustomShapeBuffers, *this);
	{
		Quad = TACRender::DrawQuad(CustomTransform);
	}
	TACRender::EndScene();
}

void UTACQuadComponent::Draw(const FVector& Location, const FVector2D& Size)
{
	CustomTransform.Location = Location;
	CustomTransform.Size = Size;

	Draw();
}

void UTACQuadComponent::GenerateCollider()
{
	CustomBoxCollider.SetMinBounds({ CustomTransform.Location.X - CustomTransform.Size.X, CustomTransform.Location.Y - CustomTransform.Size.Y, -1.f });
	CustomBoxCollider.SetMaxBounds({ CustomTransform.Location.X + CustomTransform.Size.X, CustomTransform.Location.Y + CustomTransform.Size.Y, 1.f });

	FlushPersistentDebugLines(GetWorld());
	DrawDebugSphere(GetWorld(), CustomBoxCollider.GetMinBounds(), 2.0f, 15.0f, FColor::Red, true);
	DrawDebugSphere(GetWorld(), CustomBoxCollider.GetMaxBounds(), 2.0f, 15.0f, FColor::Blue, true);
}

void UTACQuadComponent::GenerateCollider(const FVector& Location, const FVector2D& Size)
{
	CustomBoxCollider.SetLocation(Location);
	CustomBoxCollider.SetMinBounds({ Location.X - Size.X, Location.Y - Size.Y, 0.1f });
	CustomBoxCollider.SetMaxBounds({ Location.X + Size.X, Location.Y + Size.Y, 0.1f });
}
