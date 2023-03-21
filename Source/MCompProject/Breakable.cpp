#include "Breakable.h"

// Sets default values
ABreakable::ABreakable()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Create the cube mesh component
    renderer = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh");
    RootComponent = renderer;

    // Load the cube mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMeshAsset.Succeeded())
    {
        renderer->SetStaticMesh(CubeMeshAsset.Object);
    }
	
	//renderer->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // Enable collision
	//renderer->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic); // Set collision object type
	//renderer->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block); // Set collision response
	//renderer->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); // Set collision response for specific channel

	//StaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube"));
	//if (StaticMesh)
	//{
	//	renderer = NewObject<UStaticMeshComponent>(GetTransientPackage(), NAME_None);
	//	renderer->SetStaticMesh(StaticMesh);
	//	renderer->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//	renderer->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//	renderer->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	//	renderer->SetSimulatePhysics(false);
	//	renderer->SetRelativeLocation(FVector::ZeroVector);
	//	renderer->SetRelativeRotation(FRotator::ZeroRotator);
	//	renderer->SetRelativeScale3D(FVector::OneVector);
	//	renderer->RegisterComponent();
	//}
	
    cube = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    cube->SetBoxExtent(FVector(60.0f, 60.0f, 60.0f));
    cube->SetCollisionProfileName("Trigger");
    cube->SetupAttachment(RootComponent);
    
    renderer->SetSimulatePhysics(true);

    cube->OnComponentHit.AddDynamic(this, &ABreakable::OnCollision);
}

// Called when the game starts or when spawned
void ABreakable::BeginPlay()
{
    Super::BeginPlay();

    age = 0;

    Reload();
}

// Called every frame
void ABreakable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector Pos = GetActorLocation();

    age++;
    if (Pos.Size() > 1000.0f)
    {
        Destroy();
    }
}

float ABreakable::Area()
{
    if (area < 0.0f)
    {
        area = Geom::Area(polygon);
    }

    return area;
}

void ABreakable::Reload()
{
    if (renderer == nullptr) renderer = FindComponentByClass<UStaticMeshComponent>();
    if (cube == nullptr) cube = FindComponentByClass<UBoxComponent>();
    
    if (polygon.Num() == 0)
    {
        FVector scale = 0.5f * GetActorScale3D();

        polygon.Add(FVector2D(-scale.X, -scale.Y));
        polygon.Add(FVector2D(scale.X, -scale.Y));
        polygon.Add(FVector2D(scale.X, scale.Y));
        polygon.Add(FVector2D(-scale.X, scale.Y));

        thickness = 2.0f * scale.Z;

        SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
    }
    
    UStaticMesh* mesh = MeshFromPolygon(polygon, thickness);

    renderer->SetStaticMesh(mesh);
    cube->SetBoxExtent(mesh->GetBoundingBox().GetExtent());
}

void ABreakable::OnCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult)
{
    if (age > 5 && OtherComp->GetPhysicsLinearVelocity().Size() > minImpactToBreak)
    {
        FVector pnt = SweepResult.ImpactPoint;
        FVector LocalPnt = HitComp->GetComponentTransform().InverseTransformPosition(pnt);
        Break(FVector2D(LocalPnt.X, LocalPnt.Y));
    }
}

float ABreakable::NormalizedRandom(float mean, float stddev)
{
    float u1 = FMath::FRand();
    float u2 = FMath::FRand();

    float randStdNormal = FMath::Sqrt(-2.0f * FMath::Loge(u1)) *
        FMath::Sin(2.0f * PI * u2);

    return mean + stddev * randStdNormal;
}

void ABreakable::Break(FVector2D position)
{
  if (Area() > minBreakArea)
     {
         VoronoiCalculator* calc = new VoronoiCalculator();
         VoronoiClipper* clip =  new VoronoiClipper();
 
         TArray<FVector2D> sites;
  		 sites.SetNum(10);
 
         for (int i = 0; i < sites.Num(); i++)
         {
             float dist = FMath::Abs(NormalizedRandom(0.5f, 1.0f / 2.0f));
             float angle = 2.0f * PI * FMath::FRand();
 
             sites[i] = position + FVector2D(
                 dist * FMath::Cos(angle),
                 dist * FMath::Sin(angle));
         }
 
         VoronoiDiagram diagram = calc->CalculateDiagram(sites);
 
         TArray<FVector2D> clipped = TArray<FVector2D>();
 
         for (int i = 0; i < sites.Num(); i++)
         {
             clip->ClipSite(diagram, polygon, i, clipped);
             
             if (clipped.Num() > 0)
             {
                 ABreakable* bs = GetWorld()->SpawnActor<ABreakable>(GetClass(), GetTransform().GetLocation(), GetTransform().GetRotation().Rotator());
 
                 bs->SetActorScale3D(GetActorScale3D());
                 
                 bs->thickness = thickness;
                 bs->polygon.Empty();
                 bs->polygon.Append(clipped);
 
                 float childArea = bs->Area();
                 
             	UPrimitiveComponent* pc = Cast<UPrimitiveComponent>(bs);
             	if (pc != nullptr)
             	{
             		FBodyInstance* bodyInstance = pc->GetBodyInstance();
             		if (bodyInstance != nullptr)
             		{
             			FBodyInstance* parentBodyInstance = pc->GetBodyInstance();
             			if (parentBodyInstance != nullptr)
             			{
             				float mass = parentBodyInstance->GetBodyMass();
             				bodyInstance->SetMassScale(parentBodyInstance->GetMassOverride());
             				bodyInstance->SetMassOverride(mass * (childArea / area));
             			}
             		}
             	}
             }
         }
 
         SetActorHiddenInGame(true);
         Destroy();
     }
}

UStaticMesh* ABreakable::MeshFromPolygon(const TArray<FVector2D>& Polygon, const float Thickness)
{
	FMeshDescription meshDesc;
	FStaticMeshAttributes Attributes(meshDesc);
	Attributes.Register();
	
	FMeshDescriptionBuilder meshDescBuilder;
	meshDescBuilder.SetMeshDescription( &meshDesc );
	meshDescBuilder.EnablePolyGroups();
	meshDescBuilder.SetNumUVLayers(1);
	
	TArray< FVertexID > vertexIDs; vertexIDs.SetNum(3);
	TArray< FVertexInstanceID > vertexInsts;
	FVertexInstanceID instance;
	FPolygonGroupID polygonGroup = meshDescBuilder.AppendPolygonGroup();
	
	int32 count = Polygon.Num();
	
	TArray<FVector> verts;
	TArray<FVector> norms;
	TArray<int32> tris;
	
	verts.Init(FVector(0, 0, 0), 6 * count);
	norms.Init(FVector(0, 0, 0), 6 * count);
	tris.Init(0, 3 * (4 * count - 4));
	
	int32 vi = 0;
	int32 ni = 0;
	int32 ti = 0;
	
	float ext = 0.5f * Thickness;
	
	// Top
	for (int32 i = 0; i < count; i++)
	{
	    verts[vi++] = FVector(Polygon[i].X, Polygon[i].Y, ext);
	    //vertexIDs[0] = meshDescBuilder.AppendVertex(FVector(Polygon[i].X, Polygon[i].Y, ext));
	    //instance = meshDescBuilder.AppendInstance(vertexIDs[0]);
	    norms[ni++] = FVector(0, 0, 1);
	    //meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	    //meshDescBuilder.SetInstanceNormal(instance, FVector(0, 0, 1));
	    //vertexInsts.Add(instance);
	}
	
	// Bottom
	for (int32 i = 0; i < count; i++)
	{
	    verts[vi++] = FVector(Polygon[i].X, Polygon[i].Y, -ext);
	    //vertexIDs[1] = meshDescBuilder.AppendVertex(FVector(Polygon[i].X, Polygon[i].Y, -ext));
	    //instance = meshDescBuilder.AppendInstance(vertexIDs[1]);
	    norms[ni++] = FVector(0, 0, -1);
	    //meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	    //meshDescBuilder.SetInstanceNormal(instance, FVector(0, 0, -1));
	    //vertexInsts.Add(instance);
	}
	
	// Sides
	for (int32 i = 0; i < count; i++)
	{
	    int32 iNext = i == count - 1 ? 0 : i + 1;
	    
	    verts[vi++] = FVector(Polygon[i].X, Polygon[i].Y, ext);
	    verts[vi++] = FVector(Polygon[i].X, Polygon[i].Y, -ext);
	    verts[vi++] = FVector(Polygon[iNext].X, Polygon[iNext].Y, -ext);
	    verts[vi++] = FVector(Polygon[iNext].X, Polygon[iNext].Y, ext);
	
	    //vertexIDs[2] = meshDescBuilder.AppendVertex(FVector(Polygon[i].X, Polygon[i].Y, ext));
	    //instance = meshDescBuilder.AppendInstance(vertexIDs[2]);
	    
	    FVector norm = FVector::CrossProduct(FVector(Polygon[iNext].X - Polygon[i].X, Polygon[iNext].Y - Polygon[i].Y, 0), FVector(0, 0, 1)).GetSafeNormal();
	
	    norms[ni++] = norm;
	    norms[ni++] = norm;
	    norms[ni++] = norm;
	    norms[ni++] = norm;
	    //meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	    //meshDescBuilder.SetInstanceNormal(instance, norm);
	    //vertexInsts.Add(instance);
	}
	
	for (int32 vert = 2; vert < count; vert++)
	{
	    tris[ti++] = 0;
	    tris[ti++] = vert - 1;
	    tris[ti++] = vert;
	    //meshDescBuilder.AppendTriangle(vertexIDs[0], vertexIDs[1], vertexIDs[2], polygonGroup);
	}
	
	for (int32 vert = 2; vert < count; vert++)
	{
	    tris[ti++] = count;
	    tris[ti++] = count + vert;
	    tris[ti++] = count + vert - 1;
	    //meshDescBuilder.AppendTriangle(vertexIDs[2], vertexIDs[1], vertexIDs[0], polygonGroup);
	}
	
	for (int32 vert = 0; vert < count; vert++)
	{
	    int32 si = 2 * count + 4 * vert;
	
	    tris[ti++] = si;
	    tris[ti++] = si + 1;
	    tris[ti++] = si + 2;
	    //meshDescBuilder.AppendTriangle(vertexIDs[1], vertexIDs[2], vertexIDs[0], polygonGroup);
	    
	    tris[ti++] = si;
	    tris[ti++] = si + 2;
	    tris[ti++] = si + 3;
	    //meshDescBuilder.AppendTriangle(vertexIDs[0], vertexIDs[1], vertexIDs[2], polygonGroup);
	}
	
	check(ti == tris.Num());
	check(vi == verts.Num());
	check(ni == norms.Num());
	
	UStaticMesh* staticMesh = NewObject<UStaticMesh>(this);

	staticMesh->GetStaticMaterials().Add(FStaticMaterial());
	
	UStaticMesh::FBuildMeshDescriptionsParams mdParams;
	mdParams.bBuildSimpleCollision = true;
	mdParams.bFastBuild = true;
	
	// Build static mesh
	TArray<const FMeshDescription*> meshDescPtrs;
	meshDescPtrs.Emplace(&meshDesc);
	staticMesh->BuildFromMeshDescriptions(meshDescPtrs, mdParams);
	
	return staticMesh;

   // Build a simple pyramid after play has begun
	// Mesh description will hold all the geometry, uv, normals going into the static mesh
	//FMeshDescription meshDesc;
	//FStaticMeshAttributes Attributes(meshDesc);
	//Attributes.Register();
//
	//FMeshDescriptionBuilder meshDescBuilder;
	//meshDescBuilder.SetMeshDescription( &meshDesc );
	//meshDescBuilder.EnablePolyGroups();
	//meshDescBuilder.SetNumUVLayers(1);
//
	//// Create the 5 vertices needed for the shape
	//TArray< FVertexID > vertexIDs; vertexIDs.SetNum(5);
	//vertexIDs[0] = meshDescBuilder.AppendVertex(FVector(  0.0,   0.0, 100.0)); // Apex
	//vertexIDs[1] = meshDescBuilder.AppendVertex(FVector(-50.0,  50.0,   0.0)); // Corner 1
	//vertexIDs[2] = meshDescBuilder.AppendVertex(FVector(-50.0, -50.0,   0.0)); // Corner 2
	//vertexIDs[3] = meshDescBuilder.AppendVertex(FVector( 50.0, -50.0,   0.0)); // Corner 3
	//vertexIDs[4] = meshDescBuilder.AppendVertex(FVector( 50.0,  50.0,   0.0)); // Corner 4
	//
	//// Array to store all the vertex instances (3 per face)
	//TArray< FVertexInstanceID > vertexInsts;
	//
	//// Face 1 (Faces towards -X) vertex instances
	//FVertexInstanceID instance = meshDescBuilder.AppendInstance(vertexIDs[0]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(-0.7071, 0, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 1), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//instance = meshDescBuilder.AppendInstance(vertexIDs[1]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(-0.7071, 0, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 0), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//instance = meshDescBuilder.AppendInstance(vertexIDs[2]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(-0.7071, 0, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(1, 0), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//// Face 2 (Faces -Y) vertex instances
	//instance = meshDescBuilder.AppendInstance(vertexIDs[0]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(0, -0.7071, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 1), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//instance = meshDescBuilder.AppendInstance(vertexIDs[2]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(0, -0.7071, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 0), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//instance = meshDescBuilder.AppendInstance(vertexIDs[3]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(0, -0.7071, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(1, 0), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//// Face 3 (Faces towards +X) vertex instances
	//instance = meshDescBuilder.AppendInstance(vertexIDs[0]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(0.7071, 0, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 1), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//instance = meshDescBuilder.AppendInstance(vertexIDs[3]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(0.7071, 0, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 0), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//instance = meshDescBuilder.AppendInstance(vertexIDs[4]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(0.7071, 0, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(1, 0), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//// Face 4 (Faces +Y) vertex instances
	//instance = meshDescBuilder.AppendInstance(vertexIDs[0]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(0, 0.7071, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(0.0, 1.0), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//instance = meshDescBuilder.AppendInstance(vertexIDs[4]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(0, 0.7071, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(0, 0), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//instance = meshDescBuilder.AppendInstance(vertexIDs[1]);
	//meshDescBuilder.SetInstanceNormal(instance, FVector(0, 0.7071, 0.7071));
	//meshDescBuilder.SetInstanceUV(instance, FVector2D(1, 0), 0);
	//meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));
	//vertexInsts.Add(instance);
	//
	//// Allocate a polygon group
	//FPolygonGroupID polygonGroup = meshDescBuilder.AppendPolygonGroup();
	//
	//// Add triangles to mesh description
	//// Face 1 Triangle
	//meshDescBuilder.AppendTriangle(vertexInsts[2], vertexInsts[1], vertexInsts[0], polygonGroup);
	//// Face 2 Triangle
	//meshDescBuilder.AppendTriangle(vertexInsts[5], vertexInsts[4], vertexInsts[3], polygonGroup);
	//// Face 3 Triangle
	//meshDescBuilder.AppendTriangle(vertexInsts[8], vertexInsts[7], vertexInsts[6], polygonGroup);
	//// Face 4 Triangle
	//meshDescBuilder.AppendTriangle(vertexInsts[11], vertexInsts[10], vertexInsts[9], polygonGroup);
	//
	//// At least one material must be added
	//UStaticMesh* staticMesh = NewObject<UStaticMesh>(this);
	//staticMesh->GetStaticMaterials().Add(FStaticMaterial());
	//
	//UStaticMesh::FBuildMeshDescriptionsParams mdParams;
	//mdParams.bBuildSimpleCollision = true;
	//mdParams.bFastBuild = true;
	//
	//// Build static mesh
	//TArray<const FMeshDescription*> meshDescPtrs;
	//meshDescPtrs.Emplace(&meshDesc);
	//staticMesh->BuildFromMeshDescriptions(meshDescPtrs, mdParams);
//
	//return staticMesh;
}