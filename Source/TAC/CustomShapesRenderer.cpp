#include "CustomShapesRenderer.h"

namespace CustomShapesRenderer {

	static FCustomShapeBuffers* CustomShapeBuffersTarget;
	static TWeakObjectPtr<UProceduralMeshComponent> ProceduralMeshComponentTarget;

	void BeginScene(FCustomShapeBuffers& CustomShapeBuffers, UProceduralMeshComponent& ProceduralMeshComponent)
	{
		CustomShapeBuffersTarget = &CustomShapeBuffers;
		ProceduralMeshComponentTarget = &ProceduralMeshComponent;
	}

	void EndScene()
	{
		if (!ProceduralMeshComponentTarget.IsValid())
		{
			return;
		}

		ProceduralMeshComponentTarget->CreateMeshSection
		(
			0,
			CustomShapeBuffersTarget->VertexBuffer,
			CustomShapeBuffersTarget->IndexBuffer,
			CustomShapeBuffersTarget->NormalsBuffer,
			CustomShapeBuffersTarget->UV0Buffer,
			TArray<FColor>(),
			CustomShapeBuffersTarget->TangentsBuffer,
			false
		);
	}

	void AddQuadMesh(const FCustomQuadVertices& VerticesLocation, const FCustomQuadVertices& VerticesNormal)
	{
		/*
		*	QUAD VERTICES
		*
		*  v1          v3
		*   +---------+
		*	|         |
		*	|         |
		* 	+---------+
		*  v2          v4
		*/

		// ASSIGN VERTICES LOCATION
		CustomShapeBuffersTarget->VertexBuffer.Add(VerticesLocation.TopRight);
		CustomShapeBuffersTarget->VertexBuffer.Add(VerticesLocation.BottomRight);
		CustomShapeBuffersTarget->VertexBuffer.Add(VerticesLocation.TopLeft);
		CustomShapeBuffersTarget->VertexBuffer.Add(VerticesLocation.BottomLeft);

		// ASSIGN INDICES
		int32& IndexBufferCounter = CustomShapeBuffersTarget->IndexBufferCounter;
		const int32 TopRightIdx = IndexBufferCounter++;		//	0	
		const int32 BottomRightIdx = IndexBufferCounter++;	//	1
		const int32 TopLeftIdx = IndexBufferCounter++;		//	2	
		const int32 BottomLeftIdx = IndexBufferCounter++;	//	3

		CustomShapeBuffersTarget->IndexBuffer.Add(TopRightIdx);
		CustomShapeBuffersTarget->IndexBuffer.Add(BottomRightIdx);
		CustomShapeBuffersTarget->IndexBuffer.Add(TopLeftIdx);

		CustomShapeBuffersTarget->IndexBuffer.Add(BottomLeftIdx);
		CustomShapeBuffersTarget->IndexBuffer.Add(TopLeftIdx);
		CustomShapeBuffersTarget->IndexBuffer.Add(BottomRightIdx);



		// ASSIGN NORMALS

		CustomShapeBuffersTarget->NormalsBuffer.Add(VerticesNormal.TopRight);
		CustomShapeBuffersTarget->NormalsBuffer.Add(VerticesNormal.BottomRight);
		CustomShapeBuffersTarget->NormalsBuffer.Add(VerticesNormal.TopLeft);
		CustomShapeBuffersTarget->NormalsBuffer.Add(VerticesNormal.BottomLeft);

		// ASSIGN TANGENTS
		// Tangent parallel to the Y axis of UV map.
		// Is the same for every vertex.
		const FProcMeshTangent Tangent(0.0f, 1.0f, 0.0f);
		CustomShapeBuffersTarget->TangentsBuffer.Add(Tangent);
		CustomShapeBuffersTarget->TangentsBuffer.Add(Tangent);
		CustomShapeBuffersTarget->TangentsBuffer.Add(Tangent);
		CustomShapeBuffersTarget->TangentsBuffer.Add(Tangent);

		// ASSIGN UV0
		/*
		*		UV MAPPING
		*
		*	   v1        v3
		* UV01  +---------+ UV11
		*	  	|         |
		*	  	|         |
		* UV00	+---------+ UV10
		*	   v2        v4
		*/
		CustomShapeBuffersTarget->UV0Buffer.Add(FVector2D(1.0f, 1.0f)); // TR
		CustomShapeBuffersTarget->UV0Buffer.Add(FVector2D(1.0f, 0.0f)); // BR
		CustomShapeBuffersTarget->UV0Buffer.Add(FVector2D(0.0f, 1.0f)); // TL
		CustomShapeBuffersTarget->UV0Buffer.Add(FVector2D(0.0f, 0.0f)); // BL
	}

	FORCEINLINE FCustomQuadVertices GetPlaneNormals(const FCustomQuadVertices& VerticesLocation)
	{
		/*
		*	Since the Quad is flat we calculate the normal once.
		*
		*
		*		--------->
		*	 | v1        v3
		*	 | 	+---------+
		*	 | 	|         |
		*	 | 	|         |
		*	 V	+---------+
		*	   v2        v4
		*
		*	CrossProd((v3-v1), (v2-v1)).Normalized
		*/

		const FVector TopResult = VerticesLocation.TopLeft - VerticesLocation.TopRight;
		const FVector LeftResult = VerticesLocation.BottomRight - VerticesLocation.TopRight;
		const FVector CubeNormal = FVector::CrossProduct(TopResult, LeftResult).GetSafeNormal();

		return { CubeNormal , CubeNormal , CubeNormal , CubeNormal };
	}

	void AddCubeMesh(const FCustomCubeQuads& CubeData)
	{
		AddQuadMesh(CubeData.FrontQuad, GetPlaneNormals(CubeData.FrontQuad));
		AddQuadMesh(CubeData.LeftQuad, GetPlaneNormals(CubeData.LeftQuad));
		AddQuadMesh(CubeData.BackQuad, GetPlaneNormals(CubeData.BackQuad));
		AddQuadMesh(CubeData.RightQuad, GetPlaneNormals(CubeData.RightQuad));
		AddQuadMesh(CubeData.TopQuad, GetPlaneNormals(CubeData.TopQuad));
		AddQuadMesh(CubeData.BottomQuad, GetPlaneNormals(CubeData.BottomQuad));
	}

	FORCEINLINE FVector ParametricEquation(float Radius, float Longitude, float Latitude)
	{
		return FVector(
			cos(Longitude) * sin(Latitude) * Radius,
			cos(Latitude) * Radius,
			sin(Longitude) * sin(Latitude) * Radius);
	}

	FCustomQuadMeshData DrawQuad(const FCustomQuadTransform& PlaneTransform)
	{
		const FVector& Offset = PlaneTransform.Location;
		const FVector2D& VertexLocation = PlaneTransform.Size;
		const FRotator Rotator = PlaneTransform.GetRotator();

		const FVector TopRight = Rotator.RotateVector({ 0.0f, VertexLocation.X, VertexLocation.Y }) + Offset;
		const FVector BottomRight = Rotator.RotateVector({ 0.0f, VertexLocation.X, -VertexLocation.Y }) + Offset;
		const FVector TopLeft = Rotator.RotateVector({ 0.0f, -VertexLocation.X, VertexLocation.Y }) + Offset;
		const FVector BottomLeft = Rotator.RotateVector({ 0.0f, -VertexLocation.X, -VertexLocation.Y }) + Offset;

		const FCustomQuadVertices& QuadVertices = { TopRight, BottomRight, TopLeft, BottomLeft };

		const int32_t ReturnCustomPlaneDataStartIdx = CustomShapeBuffersTarget->VertexBuffer.Num();


		AddQuadMesh(QuadVertices, GetPlaneNormals(QuadVertices));

		return
		{
			(FCustomQuadVertices*)(&CustomShapeBuffersTarget->VertexBuffer[ReturnCustomPlaneDataStartIdx]),
			(FCustomQuadVertices*)(&CustomShapeBuffersTarget->NormalsBuffer[ReturnCustomPlaneDataStartIdx])
		};
	}

	FCustomCubeMeshData DrawCube(const FCustomCubeTransform& CubeTransform)
	{
		const FVector& Offset = CubeTransform.Location;
		const FVector& VertexLocation = CubeTransform.Size;
		const FRotator& Rotator = CubeTransform.GetRotator();

		const FVector FrontTopRight = Rotator.RotateVector({ VertexLocation.X, VertexLocation.Y, VertexLocation.Z }) + Offset;	
		const FVector FrontBottomRight = Rotator.RotateVector({ VertexLocation.X, VertexLocation.Y, -VertexLocation.Z }) + Offset;	
		const FVector FrontTopLeft = Rotator.RotateVector({ VertexLocation.X, -VertexLocation.Y, VertexLocation.Z }) + Offset;
		const FVector FrontBottomLeft = Rotator.RotateVector({ VertexLocation.X, -VertexLocation.Y, -VertexLocation.Z }) + Offset;

		const FVector LeftTopLeft = Rotator.RotateVector({ -VertexLocation.X, -VertexLocation.Y, VertexLocation.Z }) + Offset;
		const FVector LeftBottomLeft = Rotator.RotateVector({ -VertexLocation.X, -VertexLocation.Y, -VertexLocation.Z }) + Offset;

		const FVector BackTopRight = Rotator.RotateVector({ -VertexLocation.X, VertexLocation.Y, VertexLocation.Z }) + Offset;
		const FVector BackBottomRight = Rotator.RotateVector({ -VertexLocation.X, VertexLocation.Y, -VertexLocation.Z }) + Offset;

		const int32_t ReturnCustomCubeDataStartIdx = CustomShapeBuffersTarget->VertexBuffer.Num();

		AddCubeMesh(
		{
			{ FrontTopRight, FrontBottomRight, FrontTopLeft, FrontBottomLeft },		// FRONT QUAD
			{ FrontTopLeft, FrontBottomLeft, LeftTopLeft, LeftBottomLeft },			// LEFT QUAD
			{ LeftTopLeft, LeftBottomLeft, BackTopRight, BackBottomRight },			// BACK QUAD
			{ BackTopRight, BackBottomRight, FrontTopRight, FrontBottomRight },		// RIGHT QUAD
			{ BackTopRight, FrontTopRight, LeftTopLeft, FrontTopLeft },				// TOP QUAD
			{ LeftBottomLeft, FrontBottomLeft, BackBottomRight, FrontBottomRight}	// BOTTOM QUAD
		});


		return 
		{ 
			(FCustomCubeQuads*)(&CustomShapeBuffersTarget->VertexBuffer[ReturnCustomCubeDataStartIdx]),
			(FCustomCubeQuads*)(&CustomShapeBuffersTarget->NormalsBuffer[ReturnCustomCubeDataStartIdx])
		};
	}

	void DrawSphere(const FCustomSphereTransform& SphereTransform, int32 LongitudeSteps, int32 LatitudeSteps)
	{
		const float StartLongitude = 0, StartLatitude = 0;
		const float EndLongitude = PI * 2, EndLatitude = PI;

		const float StepLongitude = (EndLongitude - StartLongitude) / LongitudeSteps;
		const float StepLatitude = (EndLatitude - StartLatitude) / LatitudeSteps;

		for (int32 i = 0; i < LongitudeSteps; ++i)
		{
			for (int32 j = 0; j < LatitudeSteps; ++j)
			{
				const float Longitude = i * StepLongitude + StartLongitude;
				const float Latitude = j * StepLatitude + StartLatitude;

				const float LongitudeNext = (i + 1 == LongitudeSteps) ? EndLongitude : (i + 1) * StepLongitude + StartLongitude;
				const float LatitudeNext = (j + 1 == LatitudeSteps) ? EndLatitude : (j + 1) * StepLatitude + StartLatitude;

				const FRotator Rotator = SphereTransform.GetRotator();

				const FCustomQuadVertices SphereQuads =
				{
					{ Rotator.RotateVector(ParametricEquation(SphereTransform.Radius, Longitude, Latitude))} ,
					{ Rotator.RotateVector(ParametricEquation(SphereTransform.Radius, Longitude, LatitudeNext))},
					{ Rotator.RotateVector(ParametricEquation(SphereTransform.Radius, LongitudeNext, Latitude))},
					{ Rotator.RotateVector(ParametricEquation(SphereTransform.Radius, LongitudeNext, LatitudeNext))}
				};

				const FCustomQuadVertices VerticesLocation =
				{
					SphereQuads.TopRight + SphereTransform.Location,
					SphereQuads.BottomRight + SphereTransform.Location,
					SphereQuads.TopLeft + SphereTransform.Location,
					SphereQuads.BottomLeft + SphereTransform.Location
				};

				// For spheres, the normal is just the normalized value of each vertex.
				const FCustomQuadVertices VerticesNormals =
				{
					SphereQuads.TopRight.GetSafeNormal(),
					SphereQuads.BottomRight.GetSafeNormal(),
					SphereQuads.TopLeft.GetSafeNormal(),
					SphereQuads.BottomLeft.GetSafeNormal()
				};

				AddQuadMesh(VerticesLocation, VerticesNormals);
			}
		}
	}
}