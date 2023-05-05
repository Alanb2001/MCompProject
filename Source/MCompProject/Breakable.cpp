#include "Breakable.h"
#include "Geom.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"
#include "StaticMeshDescription.h"
#include "StaticMeshOperations.h"
#include "VoronoiCalculator.h"
#include "VoronoiClipper.h"

// Sets default values
ABreakable::ABreakable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create the cube mesh component
	Renderer = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh", false);
	SetRootComponent(Renderer);
	
	// Load the cube mesh
	if (static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube")); CubeMeshAsset.Succeeded())
	{
	  Renderer->SetStaticMesh(CubeMeshAsset.Object);
	}
	
	Cube = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	Cube->SetBoxExtent(FVector(60.0f, 60.0f, 60.0f));
	Cube->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Cube->SetCollisionProfileName("Trigger");
	Cube->SetNotifyRigidBodyCollision(true);
	Cube->SetupAttachment(RootComponent);
	
	Renderer->SetSimulatePhysics(true);

	Cube->OnComponentHit.AddDynamic(this, &ABreakable::OnCollision);
}

// Called when the game starts or when spawned
void ABreakable::BeginPlay()
{
    Super::BeginPlay();

    Age = 0;

    Reload();
}

// Called every frame
void ABreakable::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
	
    Age++;
    if (GetActorLocation().Size() > 1000.0f)
    {
        Destroy();
    }
}

float ABreakable::Area()
{
    if (AreaMv < 0.0f)
    {
        AreaMv = FGeom::Area(Polygon);
    }

    return AreaMv;
}

void ABreakable::Reload()
{
    if (Renderer == nullptr) Renderer = FindComponentByClass<UStaticMeshComponent>();
    if (Cube == nullptr) Cube = FindComponentByClass<UBoxComponent>();
    
    if (Polygon.Num() == 0)
    {
	    const FVector Scale = 0.5f * GetActorScale3D();

        Polygon.Add(FVector2D(-Scale.X, -Scale.Y));
        Polygon.Add(FVector2D(Scale.X, -Scale.Y));
        Polygon.Add(FVector2D(Scale.X, Scale.Y));
        Polygon.Add(FVector2D(-Scale.X, Scale.Y));

        Thickness = 2.0f * Scale.Z;

        SetActorScale3D(FVector::OneVector);
    }
	
    UStaticMesh* Mesh = MeshFromPolygon(Polygon, Thickness);
	
	Renderer->SetStaticMesh(Mesh);
	Cube->SetBoxExtent(FVector(61.0f, 61.0f, 61.0f));
}

void ABreakable::OnCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult)
{
    if (Age > 5 && OtherComp->GetPhysicsLinearVelocity().Size() > MinImpactToBreak)
    {
	    const FVector Pnt = SweepResult.ImpactPoint;
	    const FVector LocalPnt = HitComp->GetComponentTransform().InverseTransformPosition(Pnt);
        Break(FVector2D(LocalPnt.X, LocalPnt.Y));
    }
}

float ABreakable::NormalizedRandom(const float Mean, const float Stddev) const
{
	const float U1 = FMath::FRand();
	const float U2 = FMath::FRand();

	const float RandStdNormal = FMath::Sqrt(-2.0f * FMath::Loge(U1)) *
        FMath::Sin(2.0f * PI * U2);

    return Mean + Stddev * RandStdNormal;
}

void ABreakable::Break(const FVector2D Position)
{
	if (Area() > MinBreakArea)
	{
         FVoronoiCalculator Calc = FVoronoiCalculator();
         FVoronoiClipper Clip = FVoronoiClipper();
 
         TArray<FVector2D> Sites;
  		 Sites.SetNum(10);
 
         for (int i = 0; i < Sites.Num(); i++)
         {
	         const float Dist = FMath::Abs(NormalizedRandom(0.5f, 1.0f / 2.0f));
	         const float Angle = 2.0f * PI * FMath::FRand();
 
             Sites[i] = Position + FVector2D(
                 Dist * FMath::Cos(Angle),
                 Dist * FMath::Sin(Angle));
         }
 
         FFVoronoiDiagram Diagram = Calc.CalculateDiagram(Sites);

		 TArray<FVector2D> Clipped = TArray<FVector2D>();
		
         for (int i = 0; i < Sites.Num(); i++)
         {
         	Clip.ClipSite(Diagram, Polygon, i, Clipped);
         	
         	if (Clipped.Num() > 0)
         	{
         		ABreakable* BS = GetWorld()->SpawnActorDeferred<ABreakable>(GetClass(), GetTransform());
         		BS->FinishSpawning(GetTransform());
         		BS->SetActorScale3D(GetActorScale3D());
             	
         		BS->Thickness = Thickness;
         		BS->Polygon.Empty();
         		BS->Polygon = Clipped;
         		BS->Reload();
         	}
         }
 
         SetActorHiddenInGame(true);
         Destroy();
	}
}

UStaticMesh* ABreakable::MeshFromPolygon(const TArray<FVector2D>& Polygon1, const float Thickness1)
{
	const int Count = Polygon1.Num();
	
	TArray<FVector> Verts;
	TArray<FVector> Norms;
	TArray<int> Tris;
	
	Verts.Init(FVector(0, 0, 0), 6 * Count);
	Norms.Init(FVector(0, 0, 0), 6 * Count);
	Tris.Init(0, 3 * (4 * Count - 4));
	
	int VI = 0;
	int Ni = 0;
	int Ti = 0;
	
	const float EXT = 0.5f * Thickness1;
	
	FMeshDescription MeshDesc;
	FStaticMeshAttributes Attributes(MeshDesc);
	Attributes.Register();
	
	FMeshDescriptionBuilder MeshDescBuilder;
	MeshDescBuilder.SetMeshDescription( &MeshDesc );
	MeshDescBuilder.EnablePolyGroups();
	MeshDescBuilder.SetNumUVLayers(1);
	
	TArray< FVertexID > VertexIDs;
	VertexIDs.SetNum(Verts.Num());
	TArray< FVertexInstanceID > VertexInstanceIds;
	FVertexInstanceID Instance;
	FPolygonGroupID PolygonGroup = MeshDescBuilder.AppendPolygonGroup();

	// Top
	//for (int i = 0; i < Count; i++)
	//{
	//	VertexIDs[i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[i].X * 120, Polygon1[i].Y * 120, EXT * 120));
	//	Instance = MeshDescBuilder.AppendInstance(VertexIDs[i]);
	//	MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//	MeshDescBuilder.SetInstanceNormal(Instance, Norms[Ni++] = FVector(0, 0, 1));
	//	VertexInstanceIds.Add(Instance);
	//}
	
	// Top
	for (int i = 0; i < Count; i++)
	{
	    VertexIDs[i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[i].X * 120, Polygon1[i].Y * 120, -EXT * 120));
	    Instance = MeshDescBuilder.AppendInstance(VertexIDs[i]);
	    MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	    MeshDescBuilder.SetInstanceNormal(Instance,  Norms[Ni++] = FVector(0, 0, -1));
	    VertexInstanceIds.Add(Instance);
	}

	// Bottom
	//for (int i = 0; i < Count; i++)
	//{
	//	VertexIDs[i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[i].X * 120, Polygon1[i].Y * 120, -EXT * 120));
	//	Instance = MeshDescBuilder.AppendInstance(VertexIDs[i]);
	//	MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//	MeshDescBuilder.SetInstanceNormal(Instance, Norms[Ni++] = FVector(0, 0, -1));
	//	VertexInstanceIds.Add(Instance);
	//}
	
	// Bottom
	for (int i = 0; i < Count; i++)
	{
	    VertexIDs[i] = MeshDescBuilder.AppendVertex( Verts[VI++] = FVector(Polygon1[i].X * 120, Polygon1[i].Y * 120, EXT * 120));
	    Instance = MeshDescBuilder.AppendInstance(VertexIDs[i]);
	    MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	    MeshDescBuilder.SetInstanceNormal(Instance,  Norms[Ni++] = FVector(0, 0, 1));
	    VertexInstanceIds.Add(Instance);
	}
	
	// Sides
    //for (int i = 0; i < Count; i++)
    //{
    //	const int INext = i == Count - 1 ? 0 : i + 1;
    //
    //	const FVector Norm = FVector::CrossProduct(FVector(Polygon1[INext].X - Polygon1[i].X, Polygon1[INext].Y - Polygon1[i].Y, 0), FVector(0, 0, 1)).GetSafeNormal();
    //
    //	VertexIDs[i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[i].X * 120 - EXT * Norm.X * 120, Polygon1[i].Y * 120 - EXT * Norm.Y * 120, EXT * 120));
    //	Instance = MeshDescBuilder.AppendInstance(VertexIDs[i]);
    //	//MeshDescBuilder.AppendUV(FVector2D(Polygon1[i].X, Polygon1[i].Y), Instance);
    //	MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
    //	MeshDescBuilder.SetInstanceNormal(Instance, Norms[Ni++] = Norm);
    //	VertexInstanceIds.Add(Instance);
    //
    //	VertexIDs[Count + i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[i].X * 120 + EXT * Norm.X * 120, Polygon1[i].Y * 120 + EXT * Norm.Y * 120, EXT * 120));
    //	Instance = MeshDescBuilder.AppendInstance(VertexIDs[Count + i]);
    //	//MeshDescBuilder.AppendUV(FVector2D(Polygon1[i].X, Polygon1[i].Y), Instance);
    //	MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
    //	MeshDescBuilder.SetInstanceNormal(Instance, Norms[Ni++] = Norm);
    //	VertexInstanceIds.Add(Instance);
    //
    //	VertexIDs[2 * Count + i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[i].X * 120 - EXT * Norm.X * 120, Polygon1[i].Y * 120 - EXT * Norm.Y * 120, -EXT * 120));
    //	Instance = MeshDescBuilder.AppendInstance(VertexIDs[2 * Count + i]);
    //	//MeshDescBuilder.AppendUV(FVector2D(Polygon1[i].X, Polygon1[i].Y), Instance);
    //	MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
    //	MeshDescBuilder.SetInstanceNormal(Instance, Norms[Ni++] = -Norm);
    //	VertexInstanceIds.Add(Instance);
    //
    //	VertexIDs[3 * Count + i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[i].X * 120 + EXT * Norm.X * 120, Polygon1[i].Y * 120 + EXT * Norm.Y * 120, -EXT * 120));
    //	Instance = MeshDescBuilder.AppendInstance(VertexIDs[3 * Count + i]);
    //	//MeshDescBuilder.AppendUV(FVector2D(Polygon1[i].X, Polygon1[i].Y), Instance);
    //	MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
    //	MeshDescBuilder.SetInstanceNormal(Instance, Norms[Ni++] = -Norm);
    //	VertexInstanceIds.Add(Instance);
    //}
	
	// Sides
	for (int i = 0; i < Count; i++)
	{
		const int INext = i == Count - 1 ? 0 : i + 1;
	
	    const FVector Norm = FVector::CrossProduct(FVector(Polygon1[INext].X - Polygon1[i].X, Polygon1[INext].Y - Polygon1[i].Y, 0), FVector(0, 0, 1)).GetSafeNormal();
		
		VertexIDs[i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[i].X * 120, Polygon1[i].Y * 120, -EXT * 120));
		Instance = MeshDescBuilder.AppendInstance(VertexIDs[i]);
	    MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	    MeshDescBuilder.SetInstanceNormal(Instance, Norms[Ni++] = Norm);
		VertexInstanceIds.Add(Instance);
		
		VertexIDs[i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[i].X * 120, Polygon1[i].Y * 120, EXT * 120));
		Instance = MeshDescBuilder.AppendInstance(VertexIDs[i]);
		MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
		MeshDescBuilder.SetInstanceNormal(Instance, Norms[Ni++] = Norm);
		VertexInstanceIds.Add(Instance);
		
		VertexIDs[i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[INext].X * 120, Polygon1[INext].Y * 120, EXT * 120));
		Instance = MeshDescBuilder.AppendInstance(VertexIDs[i]);
		MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
		MeshDescBuilder.SetInstanceNormal(Instance, Norms[Ni++] = Norm);
		VertexInstanceIds.Add(Instance);
		
		VertexIDs[i] = MeshDescBuilder.AppendVertex(Verts[VI++] = FVector(Polygon1[INext].X * 120, Polygon1[INext].Y * 120, -EXT * 120));
		Instance = MeshDescBuilder.AppendInstance(VertexIDs[i]);
		MeshDescBuilder.SetInstanceColor(Instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
		MeshDescBuilder.SetInstanceNormal(Instance, Norms[Ni++] = Norm);
	    VertexInstanceIds.Add(Instance);
	}

	// Top
	//for (int i = 0; i < Count - 2; i++)
	//{
	//	MeshDescBuilder.AppendTriangle(VertexInstanceIds[Tris[Ti++] = VertexIDs[0]], VertexInstanceIds[Tris[Ti++] = VertexIDs[i + 1]], VertexInstanceIds[Tris[Ti++] = VertexIDs[i + 2]], PolygonGroup);
	//	//Tris[Ti++] = VertexIDs[0];
	//	//Tris[Ti++] = VertexIDs[i + 1];
	//	//Tris[Ti++] = VertexIDs[i + 2];
	//}

	for (int Vert = 2; Vert < Count; Vert++)
	{
	    MeshDescBuilder.AppendTriangle(VertexInstanceIds[Tris[Ti++] = 0], VertexInstanceIds[Tris[Ti++] = Vert - 1], VertexInstanceIds[Tris[Ti++] = Vert], PolygonGroup);
	}

	// Bottom
	//for (int i = 0; i < Count - 2; i++)
	//{
	//	MeshDescBuilder.AppendTriangle(VertexInstanceIds[Tris[Ti++] = VertexIDs[Count]], VertexInstanceIds[Tris[Ti++] = VertexIDs[2 * Count + i + 2]], VertexInstanceIds[Tris[Ti++] = VertexIDs[2 * Count + i + 1]], PolygonGroup);
	//	//Tris[Ti++] = VertexIDs[Count];
	//	//Tris[Ti++] = VertexIDs[2 * Count + i + 2];
	//	//Tris[Ti++] = VertexIDs[2 * Count + i + 1];
	//}
	
	for (int Vert = 2; Vert < Count; Vert++)
	{
	    MeshDescBuilder.AppendTriangle(VertexInstanceIds[Tris[Ti++] = Count], VertexInstanceIds[Tris[Ti++] = Count + Vert], VertexInstanceIds[Tris[Ti++] = Count + Vert - 1], PolygonGroup);
	}
	
	// Sides
	//for (int i = 0; i < Count - 2; i++)
	//{
	//	MeshDescBuilder.AppendTriangle(VertexInstanceIds[Tris[Ti++] = VertexIDs[Count]], VertexInstanceIds[Tris[Ti++] = VertexIDs[Count + 1]], VertexInstanceIds[Tris[Ti++] = VertexIDs[Count + 2]], PolygonGroup);
	//	//Tris[Ti++] = VertexIDs[Count];
	//	//Tris[Ti++] = VertexIDs[Count + 1];
	//	//Tris[Ti++] = VertexIDs[Count + 2];

	//	MeshDescBuilder.AppendTriangle(VertexInstanceIds[Tris[Ti++] = VertexIDs[Count]], VertexInstanceIds[Tris[Ti++] = VertexIDs[Count + 2]], VertexInstanceIds[Tris[Ti++] = VertexIDs[Count + 3]], PolygonGroup);
	//	//Tris[Ti++] = VertexIDs[Count];
	//	//Tris[Ti++] = VertexIDs[Count + 2];
	//	//Tris[Ti++] = VertexIDs[Count + 3];
	//}
	
	
	for (int Vert = 0; Vert < Count; Vert++)
	{
		const int Si = 2 * Count + 4 * Vert;
		
	    MeshDescBuilder.AppendTriangle(VertexInstanceIds[Tris[Ti++] = Si], VertexInstanceIds[Tris[Ti++] = Si + 1], VertexInstanceIds[Tris[Ti++] = Si + 2], PolygonGroup);
		
	    MeshDescBuilder.AppendTriangle(VertexInstanceIds[Tris[Ti++] = Si], VertexInstanceIds[Tris[Ti++] = Si + 2], VertexInstanceIds[Tris[Ti++] = Si + 3], PolygonGroup);
	}
	
	check(Ti == Tris.Num());
	check(VI == Verts.Num());
	check(Ni == Norms.Num());
	
	UStaticMesh* StaticMesh = NewObject<UStaticMesh>(this);
	StaticMesh->GetStaticMaterials().Add(FStaticMaterial());
	
	UStaticMesh::FBuildMeshDescriptionsParams MDParams;
	MDParams.bBuildSimpleCollision = true;
	MDParams.bFastBuild = true;
	
	// Build static mesh
	TArray<const FMeshDescription*> MeshDescPtr;
	MeshDescPtr.Emplace(&MeshDesc);
	StaticMesh->BuildFromMeshDescriptions(MeshDescPtr, MDParams);
	
	// Create the mesh
	UStaticMesh* Mesh = NewObject<UStaticMesh>();
	Mesh->InitResources();
	
	return StaticMesh;
	
}