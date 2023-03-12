#include "GridMeshDescription.h"

// Sets default values
AGridMeshDescription::AGridMeshDescription()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGridMeshDescription::BeginPlay()
{
	Super::BeginPlay();
	
	build_grid_mesh_description(20.0f, 20.0f, 20.0f, 20.0f);
}

// Called every frame
void AGridMeshDescription::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FMeshDescription AGridMeshDescription::build_grid_mesh_description(int x_count, int y_count, float x_scale, float y_scale)
{
	FMeshDescription mesh_desc;

	FStaticMeshAttributes attributes(mesh_desc);
	attributes.Register();

	int vertex_count = x_count * y_count;
	int quad_count = (x_count - 1) * (y_count - 1);
	int triangle_count = quad_count * 2;

	mesh_desc.ReserveNewVertices(vertex_count);
	mesh_desc.ReserveNewVertexInstances(vertex_count);

	mesh_desc.CreatePolygonGroup();
	mesh_desc.ReserveNewPolygons(triangle_count);
	mesh_desc.ReserveNewTriangles(triangle_count);
	mesh_desc.ReserveNewEdges(triangle_count * 3);

	for (int v = 0; v < vertex_count; ++v)
	{
		mesh_desc.CreateVertex();
		mesh_desc.CreateVertexInstance(v);
	}

	for (int x = 0; x < x_count - 1; ++x)
	{
		for (int y = 0; y < y_count - 1; ++y)
		{
			FVertexInstanceID v0 = y + x * x_count;
			FVertexInstanceID v1 = v0 + 1;
			FVertexInstanceID v3 = y + (x + 1) * x_count;
			FVertexInstanceID v2 = v3 + 1;

			mesh_desc.CreateEdge(v0, v1);
			mesh_desc.CreateEdge(v1, v3);
			mesh_desc.CreateEdge(v3, v0);

			mesh_desc.CreateEdge(v1, v2);
			mesh_desc.CreateEdge(v2, v3);
			mesh_desc.CreateEdge(v3, v1);
		}
	}

	mesh_desc.BuildVertexIndexers();

	for (int x = 0; x < x_count-1; ++x)
	{
		for (int y = 0; y < y_count-1; ++y)
		{
			FVertexInstanceID v0 = y + x * x_count;
			FVertexInstanceID v1 = v0 + 1;
			FVertexInstanceID v3 = y + (x + 1) * x_count;
			FVertexInstanceID v2 = v3+1;

			mesh_desc.CreateTriangle(0, { v0, v1, v3 });					
			mesh_desc.CreateTriangle(0, { v1, v2, v3 });
		}
	}

	auto positions = mesh_desc.GetVertexPositions().GetRawArray();;
	auto uvs = mesh_desc.VertexInstanceAttributes().GetAttributesRef<FVector2f>(MeshAttribute::VertexInstance::TextureCoordinate).GetRawArray();

	for (int x = 0; x < x_count; ++x)
	{
		for (int y = 0; y < y_count; ++y)
		{
			int i = y + x * x_count;

			float u = (float)y / (y_count - 1);
			float v = (float)x / (x_count - 1);

			uvs[i] = FVector2f(u, v);
			positions[i] = FVector3f((v-0.5f)*x_scale, (u-0.5f)*y_scale, 0);
		}
	}

	return mesh_desc;
}