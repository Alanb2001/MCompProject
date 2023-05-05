#include "StaticMeshRuntimeTest.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"

// Sets default values
AStaticMeshRuntimeTest::AStaticMeshRuntimeTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SMComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("smComp"), false);
	SetRootComponent(SMComp);
}

// Called when the game starts or when spawned
void AStaticMeshRuntimeTest::BeginPlay()
{
	Super::BeginPlay();

	// Build a simple pyramid after play has begun
	// Mesh description will hold all the geometry, uv, normals going into the static mesh
	FMeshDescription meshDesc;
	FStaticMeshAttributes Attributes(meshDesc);
	Attributes.Register();

	FMeshDescriptionBuilder meshDescBuilder;
	meshDescBuilder.SetMeshDescription( &meshDesc );
	meshDescBuilder.EnablePolyGroups();
	meshDescBuilder.SetNumUVLayers(1);

	// Create the 5 vertices needed for the shape
	TArray< FVertexID > vertexIDs; vertexIDs.SetNum(5);
	vertexIDs[0] = meshDescBuilder.AppendVertex(FVector(  0.0,   0.0, 100.0)); // Apex
	vertexIDs[1] = meshDescBuilder.AppendVertex(FVector(-50.0,  50.0,   0.0)); // Corner 1
	vertexIDs[2] = meshDescBuilder.AppendVertex(FVector(-50.0, -50.0,   0.0)); // Corner 2
	vertexIDs[3] = meshDescBuilder.AppendVertex(FVector( 50.0, -50.0,   0.0)); // Corner 3
	vertexIDs[4] = meshDescBuilder.AppendVertex(FVector( 50.0,  50.0,   0.0)); // Corner 4
	
	// Array to store all the vertex instances (3 per face)
	TArray< FVertexInstanceID > vertexInsts;
	
	// Face 1 (Faces towards -X) vertex instances
	FVertexInstanceID instance = meshDescBuilder.AppendInstance(vertexIDs[0]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(-0.7071, 0, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 1), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	instance = meshDescBuilder.AppendInstance(vertexIDs[1]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(-0.7071, 0, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 0), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	instance = meshDescBuilder.AppendInstance(vertexIDs[2]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(-0.7071, 0, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(1, 0), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	// Face 2 (Faces -Y) vertex instances
	instance = meshDescBuilder.AppendInstance(vertexIDs[0]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(0, -0.7071, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 1), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	instance = meshDescBuilder.AppendInstance(vertexIDs[2]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(0, -0.7071, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 0), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	instance = meshDescBuilder.AppendInstance(vertexIDs[3]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(0, -0.7071, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(1, 0), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	// Face 3 (Faces towards +X) vertex instances
	instance = meshDescBuilder.AppendInstance(vertexIDs[0]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(0.7071, 0, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 1), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	instance = meshDescBuilder.AppendInstance(vertexIDs[3]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(0.7071, 0, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 0), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	instance = meshDescBuilder.AppendInstance(vertexIDs[4]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(0.7071, 0, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(1, 0), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	// Face 4 (Faces +Y) vertex instances
	instance = meshDescBuilder.AppendInstance(vertexIDs[0]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(0, 0.7071, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(0.0, 1.0), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	instance = meshDescBuilder.AppendInstance(vertexIDs[4]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(0, 0.7071, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 0), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	instance = meshDescBuilder.AppendInstance(vertexIDs[1]);
	meshDescBuilder.SetInstanceNormal(instance, FVector(0, 0.7071, 0.7071));
	meshDescBuilder.SetInstanceUV(instance, FVector2D(1, 0), 0);
	meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	vertexInsts.Add(instance);
	
	// Allocate a polygon group
	FPolygonGroupID polygonGroup = meshDescBuilder.AppendPolygonGroup();
	
	// Add triangles to mesh description
	// Face 1 Triangle
	meshDescBuilder.AppendTriangle(vertexInsts[2], vertexInsts[1], vertexInsts[0], polygonGroup);
	// Face 2 Triangle
	meshDescBuilder.AppendTriangle(vertexInsts[5], vertexInsts[4], vertexInsts[3], polygonGroup);
	// Face 3 Triangle
	meshDescBuilder.AppendTriangle(vertexInsts[8], vertexInsts[7], vertexInsts[6], polygonGroup);
	// Face 4 Triangle
	meshDescBuilder.AppendTriangle(vertexInsts[11], vertexInsts[10], vertexInsts[9], polygonGroup);
	
	// At least one material must be added
	UStaticMesh* staticMesh = NewObject<UStaticMesh>(this);
	staticMesh->GetStaticMaterials().Add(FStaticMaterial());
	
	UStaticMesh::FBuildMeshDescriptionsParams mdParams;
	mdParams.bBuildSimpleCollision = true;
	mdParams.bFastBuild = true;
	
	// Build static mesh
	TArray<const FMeshDescription*> meshDescPtrs;
	meshDescPtrs.Emplace(&meshDesc);
	staticMesh->BuildFromMeshDescriptions(meshDescPtrs, mdParams);
	
	// Assign new static mesh to the static mesh component
	SMComp->SetStaticMesh(staticMesh);
}

// Called every frame
void AStaticMeshRuntimeTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

