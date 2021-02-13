#pragma once
#include "TACRenderSystemModule/Public/TACRenderShapesTypes.h"

namespace TACRender {

	TACRENDERSYSTEMMODULE_API void BeginScene(FTACShapeBuffers& ShapeBuffers, UProceduralMeshComponent& ProceduralMeshComponent);
	TACRENDERSYSTEMMODULE_API void EndScene();

	TACRENDERSYSTEMMODULE_API FTACQuadMeshData DrawQuad(const FTACQuadTransform& PlaneTransform);
	TACRENDERSYSTEMMODULE_API FTACCubeMeshData DrawCube(const FTACCubeTransform& CubeTransform);
	TACRENDERSYSTEMMODULE_API void DrawSphere(const FTACSphereTransform& SphereTransform, int32 LongitudeSteps = 15, int32 LatitudeSteps = 15);
}