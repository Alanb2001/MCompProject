#include "ProceduralMeshTest.h"

// Sets default values
AProceduralMeshTest::AProceduralMeshTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProcMesh");
	RootComponent = ProcMesh;
}

// Called when the game starts or when spawned
void AProceduralMeshTest::BeginPlay()
{
	Super::BeginPlay();

	CreateMesh();
}

// Called every frame
void AProceduralMeshTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProceduralMeshTest::CreateMesh()
{
	Vertices.Add(FVector(-50, 0, 50));
	Vertices.Add(FVector(-50, 0, -50));
	Vertices.Add(FVector(50, 0, 50));
	Vertices.Add(FVector(50, 0, -50));

	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(0, 1));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(1, 1));

	//Triangle1
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);

	//Triangle2
	Triangles.Add(2);
	Triangles.Add(1);
	Triangles.Add(3);
	
	ProcMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	if (Material)
	{
		ProcMesh->SetMaterial(0, Material);
	}
}