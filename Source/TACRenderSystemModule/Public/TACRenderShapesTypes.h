#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "TACRenderShapesTypes.generated.h"

USTRUCT()
struct TACRENDERSYSTEMMODULE_API FTACShapeBuffers
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
struct TACRENDERSYSTEMMODULE_API FTACTransform
{
	GENERATED_USTRUCT_BODY()

	FTACTransform() : Location(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f) {}

	FTACTransform(const FVector& Location) : Location(Location), Rotation(0.0f, 0.0f, 0.0f) {}

	FTACTransform(const FVector& Location, const FVector& Rotation) : Location(Location), Rotation(Rotation) {}

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
struct TACRENDERSYSTEMMODULE_API FTACCubeTransform : public FTACTransform
{
	GENERATED_USTRUCT_BODY()

	FTACCubeTransform() : FTACTransform(), Size(50.0f, 50.0f, 50.0f) {}
	
	FTACCubeTransform(const FVector& Location, const FVector& Rotation, const FVector& Size) : FTACTransform(Location, Rotation), Size(Size)
	{}

	FTACCubeTransform(const FVector& Location, const FVector& Rotation) : FTACTransform(Location, Rotation), Size(50.0f, 50.0f, 50.0f)
	{}

	FTACCubeTransform(const FVector& Location) : FTACTransform(Location), Size(50.0f, 50.0f, 50.0f)
	{}

	UPROPERTY(EditAnywhere)
	FVector Size;
};

USTRUCT()
struct TACRENDERSYSTEMMODULE_API FTACSphereTransform : public FTACTransform
{
	GENERATED_USTRUCT_BODY()
	
	FTACSphereTransform() : FTACTransform({ 0.0f,0.0f,0.0f }, { 90.0f, 0.0f, 0.0f }), Radius(5.0f) {}

	FTACSphereTransform(const FVector& Location, const FVector& Rotation, float Radius) : FTACTransform(Location, Rotation), Radius(Radius)
	{}

	FTACSphereTransform(const FVector& Location, const FVector& Rotation) : FTACTransform(Location, Rotation), Radius(5.0f)
	{}

	FTACSphereTransform(const FVector& Location) : FTACTransform(Location, { 90.0f,0.0f, 0.0f }), Radius(5.0f)
	{}

	FTACSphereTransform(float Radius) : FTACTransform(Location, { 90.0f,0.0f, 0.0f }), Radius(Radius)
	{}

	FTACSphereTransform(const FVector& Location, float Radius) : FTACTransform(Location, { 90.0f,0.0f, 0.0f }), Radius(Radius)
	{}

	UPROPERTY(EditAnywhere)
	float Radius;
};

USTRUCT()
struct TACRENDERSYSTEMMODULE_API FTACQuadTransform : public FTACTransform
{
	GENERATED_USTRUCT_BODY()


	FTACQuadTransform() : FTACTransform(), Size(50.0f, 50.0f) {}

	FTACQuadTransform(const FVector& Location, const FVector& Rotation, const FVector2D& Size) : FTACTransform(Location, Rotation), Size(Size)
	{}

	FTACQuadTransform(const FVector& Location, const FVector& Rotation) : FTACTransform(Location, Rotation), Size(50.0f, 50.0f)
	{}

	FTACQuadTransform(const FVector& Location) : FTACTransform(Location), Size(50.0f, 50.0f)
	{}

	UPROPERTY(EditAnywhere)
	FVector2D Size;
};

struct TACRENDERSYSTEMMODULE_API FTACQuadVertices
{
	FVector TopRight, BottomRight, TopLeft, BottomLeft;
};

struct TACRENDERSYSTEMMODULE_API FTACCubeQuads
{
	FTACQuadVertices FrontQuad, LeftQuad, BackQuad, RightQuad, TopQuad, BottomQuad;
};

struct TACRENDERSYSTEMMODULE_API FTACCubeMeshData
{
	FTACCubeQuads* Positions = nullptr;
	FTACCubeQuads* Normals = nullptr;
};

struct TACRENDERSYSTEMMODULE_API FTACQuadMeshData
{
	FTACQuadVertices* Positions = nullptr;
	FTACQuadVertices* Normals = nullptr;
};