#include "Breakable.h"

// Sets default values
ABreakable::ABreakable()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    //// Create the cube mesh component
    renderer = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh");
    RootComponent = renderer;

    // Load the cube mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMeshAsset.Succeeded())
    {
        renderer->SetStaticMesh(CubeMeshAsset.Object);
    }

    cube = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    cube->SetBoxExtent(FVector(120.0f, 120.0f, 120.0f));
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
    FVector pos = GetActorLocation();

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
}

void ABreakable::OnCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& SweepResult)
{
    if (age > 5 && OtherComp->GetPhysicsLinearVelocity().Size() > minImpactToBreak)
    {
        FVector pnt = SweepResult.ImpactPoint;
        FVector LocalPnt = HitComp->GetComponentTransform().InverseTransformPosition(pnt);
        Break(FVector2D(LocalPnt.X, LocalPnt.Y));
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "OnComponentHit Function called");
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
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "plz");
    
    if (Area() > minBreakArea)
    {
        VoronoiCalculator* calc = new VoronoiCalculator();
        VoronoiClipper* clip =  new VoronoiClipper();

        TArray<FVector2D> sites;

        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Maybe");

        for (int i = 0; i < 10; i++)
        {
            float dist = FMath::Abs(NormalizedRandom(0.5f, 1.0f / 2.0f));
            float angle = 2.0f * PI * FMath::FRand();

            sites.Add(position + FVector2D(
                dist * FMath::Cos(angle),
                dist * FMath::Sin(angle)));
            
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "No");
        }

        VoronoiDiagram diagram = calc->CalculateDiagram(sites);

        TArray<FVector2D> clipped;

        for (int i = 0; i < sites.Num(); i++)
        {
            clip->ClipSite(diagram, polygon, i, clipped);

            if (clipped.Num() > 0)
            {
                ABreakable* bs = GetWorld()->SpawnActor<ABreakable>(GetClass(), GetTransform().GetLocation(), GetTransform().GetRotation().Rotator());

                bs->thickness = thickness;
                bs->polygon.Empty();
                bs->polygon.Append(clipped);

                float childArea = bs->Area();

                UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(bs);
                if (primitiveComponent)
                {
                    float parentMass = primitiveComponent->GetMass();
                    primitiveComponent->SetMassOverrideInKg(NAME_None, parentMass * (childArea / area));

                    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, "Yes");
                }
            }
        }

        SetActorHiddenInGame(true);
        Destroy();
    }
}

UStaticMesh* ABreakable::MeshFromPolygon(const TArray<FVector2D>& Polygon, const float Thickness)
{
    int32 Count = Polygon.Num();
    TArray<FVector> Verts;
    TArray<FVector> Norms;
    TArray<int32> Tris;
    int32 Vi = 0;
    int32 Ni = 0;
    int32 Ti = 0;
    float Ext = 0.5f * Thickness;

    // Top
    for (int32 i = 0; i < Count; i++)
    {
        Verts.Add(FVector(Polygon[i].X, Polygon[i].Y, Ext));
        Norms.Add(FVector(0.f, 0.f, 1.f));
    }

    // Bottom
    for (int32 i = 0; i < Count; i++)
    {
        Verts.Add(FVector(Polygon[i].X, Polygon[i].Y, -Ext));
        Norms.Add(FVector(0.f, 0.f, -1.f));
    }

    // Sides
    for (int32 i = 0; i < Count; i++)
    {
        int32 iNext = i == Count - 1 ? 0 : i + 1;

        Verts.Add(FVector(Polygon[i].X, Polygon[i].Y, Ext));
        Verts.Add(FVector(Polygon[i].X, Polygon[i].Y, -Ext));
        Verts.Add(FVector(Polygon[iNext].X, Polygon[iNext].Y, -Ext));
        Verts.Add(FVector(Polygon[iNext].X, Polygon[iNext].Y, Ext));

        FVector Norm = FVector::CrossProduct(FVector(Polygon[iNext] - Polygon[i], 0.f), FVector(0.f, 0.f, 1.f)).GetSafeNormal();

        Norms.Add(Norm);
        Norms.Add(Norm);
        Norms.Add(Norm);
        Norms.Add(Norm);
    }

    for (int32 Vert = 2; Vert < Count; Vert++)
    {
        Tris.Add(0);
        Tris.Add(Vert - 1);
        Tris.Add(Vert);
    }

    for (int32 Vert = 2; Vert < Count; Vert++)
    {
        Tris.Add(Count);
        Tris.Add(Count + Vert);
        Tris.Add(Count + Vert - 1);
    }

    for (int32 Vert = 0; Vert < Count; Vert++)
    {
        int32 Si = 2 * Count + 4 * Vert;

        Tris.Add(Si);
        Tris.Add(Si + 1);
        Tris.Add(Si + 2);

        Tris.Add(Si);
        Tris.Add(Si + 2);
        Tris.Add(Si + 3);
    }

    check(Ti == Tris.Num());
    check(Vi == Verts.Num());

    UStaticMesh* Mesh = NewObject<UStaticMesh>();

    return Mesh;
}