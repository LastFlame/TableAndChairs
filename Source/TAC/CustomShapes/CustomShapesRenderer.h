#pragma once
#include "CustomShapesTypes.h"

namespace CustomShapesRenderer {

	void BeginScene(FCustomShapeBuffers& CustomShapeBuffers, UProceduralMeshComponent& ProceduralMeshComponent);
	void EndScene();

	FCustomQuadMeshData DrawQuad(const FCustomQuadTransform& PlaneTransform);
	FCustomCubeMeshData DrawCube(const FCustomCubeTransform& CubeTransform);
	void DrawSphere(const FCustomSphereTransform& SphereTransform, int32 LongitudeSteps = 15, int32 LatitudeSteps = 15);
}