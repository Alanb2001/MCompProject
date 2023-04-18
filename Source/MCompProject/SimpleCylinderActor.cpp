#include "SimpleCylinderActor.h"

ASimpleCylinderActor::ASimpleCylinderActor()
{
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	RootComponent = Mesh;
}

void ASimpleCylinderActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	GenerateMesh();
}

void ASimpleCylinderActor::BeginPlay()
{
	Super::BeginPlay();
	GenerateMesh();
}

void ASimpleCylinderActor::GenerateMesh() const
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	const TArray<FColor> VertexColors;

	GenerateCylinder(Vertices, Triangles, Normals, UVs, Tangents, Height, Radius, CrossSectionCount, bCapEnds, bDoubleSided, bSmoothNormals);

	Mesh->ClearAllMeshSections();
	Mesh->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, false);
}

void ASimpleCylinderActor::GenerateCylinder(TArray<FVector>& Vertices, TArray<int32>& Triangles, TArray<FVector>& Normals, TArray<FVector2D>& UVs, TArray<FProcMeshTangent>& Tangents, float HeightP, float InWidth, int32 InCrossSectionCount, bool bCapEndsP, bool bDoubleSidedP, bool bInSmoothNormals) const
{
	// -------------------------------------------------------
	// Basic setup
	int VertexIndex = 0;
	int32 NumVerts = InCrossSectionCount * 4; // InCrossSectionCount x 4 verts per face

	// Count extra vertices if double sided
	if (bDoubleSidedP)
	{
		NumVerts = NumVerts * 2;
	}

	// Count vertices for caps if set
	if (bCapEndsP)
	{
		NumVerts += 2 * (InCrossSectionCount - 1) * 3;
	}

	// Clear out the arrays passed in
	Triangles.Reset();

	Vertices.Reset();
	Vertices.AddUninitialized(NumVerts);

	Normals.Reset();
	Normals.AddUninitialized(NumVerts);

	Tangents.Reset();
	Tangents.AddUninitialized(NumVerts);

	UVs.Reset();
	UVs.AddUninitialized(NumVerts);

	// -------------------------------------------------------
	// Make a cylinder section
	const float AngleBetweenQuads = 2.0f / static_cast<float>(InCrossSectionCount) * PI;
	const float VMapPerQuad = 1.0f / static_cast<float>(InCrossSectionCount);
	FVector Offset = FVector(0, 0, HeightP);

	// Start by building up vertices that make up the cylinder sides
	for (int32 QuadIndex = 0; QuadIndex < InCrossSectionCount; QuadIndex++)
	{
		float Angle = static_cast<float>(QuadIndex) * AngleBetweenQuads;
		float NextAngle = static_cast<float>(QuadIndex + 1) * AngleBetweenQuads;

		// Set up the vertices
		FVector P0 = FVector(FMath::Cos(Angle) * InWidth, FMath::Sin(Angle) * InWidth, 0.f);
		FVector P1 = FVector(FMath::Cos(NextAngle) * InWidth, FMath::Sin(NextAngle) * InWidth, 0.f);
		FVector P2 = P1 + Offset;
		FVector P3 = P0 + Offset;

		// Set up the quad triangles
		int VertIndex1 = VertexIndex++;
		int VertIndex2 = VertexIndex++;
		int VertIndex3 = VertexIndex++;
		int VertIndex4 = VertexIndex++;

		Vertices[VertIndex1] = P0;
		Vertices[VertIndex2] = P1;
		Vertices[VertIndex3] = P2;
		Vertices[VertIndex4] = P3;

		// Now create two triangles from those four vertices
		// The order of these (clockwise/counter-clockwise) dictates which way the normal will face. 
		Triangles.Add(VertIndex4);
		Triangles.Add(VertIndex3);
		Triangles.Add(VertIndex1);

		Triangles.Add(VertIndex3);
		Triangles.Add(VertIndex2);
		Triangles.Add(VertIndex1);

		// UVs
		UVs[VertIndex1] = FVector2D(VMapPerQuad * QuadIndex, 0.0f);
		UVs[VertIndex2] = FVector2D(VMapPerQuad * (QuadIndex + 1), 0.0f);
		UVs[VertIndex3] = FVector2D(VMapPerQuad * (QuadIndex + 1), 1.0f);
		UVs[VertIndex4] = FVector2D(VMapPerQuad * QuadIndex, 1.0f);

		// Normals
		FVector NormalCurrent = FVector::CrossProduct(Vertices[VertIndex1] - Vertices[VertIndex3], Vertices[VertIndex2] - Vertices[VertIndex3]).GetSafeNormal();

		if (bInSmoothNormals)
		{
			// To smooth normals you give the vertices a different normal value than the polygon they belong to, gfx hardware then knows how to interpolate between those.
			// I do this here as an average between normals of two adjacent polygons
			// TODO re-use calculations between loop iterations (do them once and cache them!), no need to calculate same values every time :)
			float NextNextAngle = static_cast<float>(QuadIndex + 2) * AngleBetweenQuads;
			FVector P4 = FVector(FMath::Cos(NextNextAngle) * InWidth, FMath::Sin(NextNextAngle) * InWidth, 0.f);

			// p1 to p4 to p2
			FVector NormalNext = FVector::CrossProduct(P1 - P2, P4 - P2).GetSafeNormal();
			FVector AverageNormalRight = (NormalCurrent + NormalNext) / 2;
			AverageNormalRight = AverageNormalRight.GetSafeNormal();

			float PreviousAngle = static_cast<float>(QuadIndex - 1) * AngleBetweenQuads;
			FVector PMinus1 = FVector(FMath::Cos(PreviousAngle) * InWidth, FMath::Sin(PreviousAngle) * InWidth, 0.f);

			// p0 to p3 to pMinus1
			FVector NormalPrevious = FVector::CrossProduct(P0 - PMinus1, P3 - PMinus1).GetSafeNormal();
			FVector AverageNormalLeft = (NormalCurrent + NormalPrevious) / 2;
			AverageNormalLeft = AverageNormalLeft.GetSafeNormal();

			Normals[VertIndex1] = AverageNormalLeft;
			Normals[VertIndex2] = AverageNormalRight;
			Normals[VertIndex3] = AverageNormalRight;
			Normals[VertIndex4] = AverageNormalLeft;
		}
		else
		{
			// If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
			Normals[VertIndex1] = NormalCurrent;
			Normals[VertIndex2] = NormalCurrent;
			Normals[VertIndex3] = NormalCurrent;
			Normals[VertIndex4] = NormalCurrent;
		}

		// Tangents (perpendicular to the surface)
		FVector SurfaceTangent = P0 - P1;
		SurfaceTangent = SurfaceTangent.GetSafeNormal();
		Tangents[VertIndex1] = FProcMeshTangent(SurfaceTangent, true);
		Tangents[VertIndex2] = FProcMeshTangent(SurfaceTangent, true);
		Tangents[VertIndex3] = FProcMeshTangent(SurfaceTangent, true);
		Tangents[VertIndex4] = FProcMeshTangent(SurfaceTangent, true);

		// If double sides, create extra polygons but face the normals the other way.
		if (bDoubleSidedP)
		{
			VertIndex1 = VertexIndex++;
			VertIndex2 = VertexIndex++;
			VertIndex3 = VertexIndex++;
			VertIndex4 = VertexIndex++;

			Vertices[VertIndex1] = P0;
			Vertices[VertIndex2] = P1;
			Vertices[VertIndex3] = P2;
			Vertices[VertIndex4] = P3;

			Triangles.Add(VertIndex1);
			Triangles.Add(VertIndex3);
			Triangles.Add(VertIndex4);

			Triangles.Add(VertIndex2);
			Triangles.Add(VertIndex3);
			Triangles.Add(VertIndex4);
		}

		if (QuadIndex != 0 && bCapEndsP)
		{
			// Cap is closed by triangles that start at 0, then use the points at the angles for the other corners

			// Bottom
			FVector CapVertex0 = FVector(FMath::Cos(0.0f) * InWidth, FMath::Sin(0.0f) * InWidth, 0.f);
			FVector CapVertex1 = FVector(FMath::Cos(Angle) * InWidth, FMath::Sin(Angle) * InWidth, 0.f);
			FVector CapVertex2 = FVector(FMath::Cos(NextAngle) * InWidth, FMath::Sin(NextAngle) * InWidth, 0.f);

			VertIndex1 = VertexIndex++;
			VertIndex2 = VertexIndex++;
			VertIndex3 = VertexIndex++;
			Vertices[VertIndex1] = CapVertex0;
			Vertices[VertIndex2] = CapVertex1;
			Vertices[VertIndex3] = CapVertex2;

			Triangles.Add(VertIndex1);
			Triangles.Add(VertIndex2);
			Triangles.Add(VertIndex3);

			FVector2D UV1 = FVector2D(FMath::Sin(0.0f), FMath::Cos(0.0f));
			FVector2D UV2 = FVector2D(FMath::Sin(Angle), FMath::Cos(Angle));
			FVector2D UV3 = FVector2D(FMath::Sin(NextAngle), FMath::Cos(NextAngle));

			UVs[VertIndex1] = UV1;
			UVs[VertIndex2] = UV2;
			UVs[VertIndex3] = UV3;

			// Top
			CapVertex0 = CapVertex0 + Offset;
			CapVertex1 = CapVertex1 + Offset;
			CapVertex2 = CapVertex2 + Offset;

			VertIndex1 = VertexIndex++;
			VertIndex2 = VertexIndex++;
			VertIndex3 = VertexIndex++;
			Vertices[VertIndex1] = CapVertex0;
			Vertices[VertIndex2] = CapVertex1;
			Vertices[VertIndex3] = CapVertex2;

			Triangles.Add(VertIndex3);
			Triangles.Add(VertIndex2);
			Triangles.Add(VertIndex1);

			UVs[VertIndex1] = UV1;
			UVs[VertIndex2] = UV2;
			UVs[VertIndex3] = UV3;
		}
	}
}
