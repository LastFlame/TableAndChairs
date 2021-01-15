#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "CustomShapesTypes.generated.h"

USTRUCT()
struct FCustomShapeBuffers
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> VertexBuffer;

	UPROPERTY(VisibleAnywhere)
	int32 IndexBufferCounter;

	UPROPERTY(VisibleAnywhere)
	TArray<int32> IndexBuffer;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> NormalsBuffer;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector2D> UV0Buffer;

	UPROPERTY(VisibleAnywhere)
	TArray<FProcMeshTangent> TangentsBuffer;

	void Reset()
	{
		VertexBuffer.Reset();
		IndexBufferCounter = 0;
		IndexBuffer.Reset();
		NormalsBuffer.Reset();
		UV0Buffer.Reset();
		TangentsBuffer.Reset();
	}
};

USTRUCT()
struct FCustomTransform
{
	GENERATED_USTRUCT_BODY()

	FCustomTransform() : Location(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f) {}

	FCustomTransform(const FVector& Location) : Location(Location), Rotation(0.0f, 0.0f, 0.0f) {}

	FCustomTransform(const FVector& Location, const FVector& Rotation) : Location(Location), Rotation(Rotation) {}

	UPROPERTY(EditAnywhere)
	FVector Location;

	UPROPERTY(EditAnywhere)
	FVector Rotation;

	FRotator GetRotator() const
	{
		return FRotator(Rotation.Y, Rotation.Z, Rotation.X);
	}
};


USTRUCT()
struct FCustomCubeTransform : public FCustomTransform
{
	GENERATED_USTRUCT_BODY()

	FCustomCubeTransform() : FCustomTransform(), Size(50.0f, 50.0f, 50.0f) {}
	
	FCustomCubeTransform(const FVector& Location, const FVector& Rotation, const FVector& Size) : FCustomTransform(Location, Rotation), Size(Size)
	{}

	FCustomCubeTransform(const FVector& Location, const FVector& Rotation) : FCustomTransform(Location, Rotation), Size(50.0f, 50.0f, 50.0f)
	{}

	FCustomCubeTransform(const FVector& Location) : FCustomTransform(Location), Size(50.0f, 50.0f, 50.0f)
	{}

	UPROPERTY(EditAnywhere)
	FVector Size;
};

USTRUCT()
struct FCustomSphereTransform : public FCustomTransform
{
	GENERATED_USTRUCT_BODY()
	
	FCustomSphereTransform() : FCustomTransform({ 0.0f,0.0f,0.0f }, { 90.0f, 0.0f, 0.0f }), Radius(5.0f) {}

	FCustomSphereTransform(const FVector& Location, const FVector& Rotation, float Radius) : FCustomTransform(Location, Rotation), Radius(Radius)
	{}

	FCustomSphereTransform(const FVector& Location, const FVector& Rotation) : FCustomTransform(Location, Rotation), Radius(5.0f)
	{}

	FCustomSphereTransform(const FVector& Location) : FCustomTransform(Location, { 90.0f,0.0f, 0.0f }), Radius(5.0f)
	{}

	FCustomSphereTransform(float Radius) : FCustomTransform(Location, { 90.0f,0.0f, 0.0f }), Radius(Radius)
	{}

	FCustomSphereTransform(const FVector& Location, float Radius) : FCustomTransform(Location, { 90.0f,0.0f, 0.0f }), Radius(Radius)
	{}

	UPROPERTY(EditAnywhere)
	float Radius;
};

USTRUCT()
struct FCustomQuadTransform : public FCustomTransform
{
	GENERATED_USTRUCT_BODY()


	FCustomQuadTransform() : FCustomTransform(), Size(50.0f, 50.0f) {}

	FCustomQuadTransform(const FVector& Location, const FVector& Rotation, const FVector2D& Size) : FCustomTransform(Location, Rotation), Size(Size)
	{}

	FCustomQuadTransform(const FVector& Location, const FVector& Rotation) : FCustomTransform(Location, Rotation), Size(50.0f, 50.0f)
	{}

	FCustomQuadTransform(const FVector& Location) : FCustomTransform(Location), Size(50.0f, 50.0f)
	{}

	UPROPERTY(EditAnywhere)
	FVector2D Size;
};

struct FCustomQuadVertices
{
	FVector TopRight, BottomRight, TopLeft, BottomLeft;
};

struct FCustomCubeQuads
{
	FCustomQuadVertices FrontQuad, LeftQuad, BackQuad, RightQuad, TopQuad, BottomQuad;
};

struct FCustomCubeMeshData
{
	FCustomCubeQuads* Positions;
	FCustomCubeQuads* Normals;
};

struct FCustomQuadMeshData
{
	FCustomQuadVertices* Positions;
	FCustomQuadVertices* Normals;
};