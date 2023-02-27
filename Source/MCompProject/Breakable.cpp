// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable.h"
#include <cassert>

// Sets default values
ABreakable::ABreakable()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Create the cube mesh component
    CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("CubeMesh");
    RootComponent = CubeMesh;

    // Load the cube mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMeshAsset.Succeeded())
    {
        CubeMesh->SetStaticMesh(CubeMeshAsset.Object);
    }

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

    if (filter == nullptr) filter = FindComponentByClass<UStaticMeshComponent>();
    if (renderer == nullptr) renderer = FindComponentByClass<UStaticMeshComponent>();
    if (collider == nullptr) collider = FindComponentByClass<UStaticMeshComponent>();
    //if (rigidbody == nullptr) rigidbody = FindComponentByClass<URigidbodyComponent>();

    if (polygon.Num() == 0)
    {
        FVector scale = 0.5f * GetActorScale3D();

        polygon.Add(FVector2D(-scale.X, -scale.Y));
        polygon.Add(FVector2D(scale.X, -scale.Y));
        polygon.Add(FVector2D(scale.X, scale.Y));
        polygon.Add(FVector2D(-scale.X, scale.Y));

        thickness = 2.0f * scale.Z;

        SetActorScale3D(FVector(1.f, 1.f, 1.f));
    }

    //UStaticMesh* mesh = MeshFromPolygon(polygon, thickness);

    //filter->SetStaticMesh(mesh);
    //collider->SetStaticMesh(mesh);
}

//UStaticMesh* ABreakable::MeshFromPolygon(const TArray<FVector2D>& Polygon, const float Thickness)
//{
//    //int32 Count = Polygon.Num();
//    //TArray<FVector> Verts;
//    //TArray<FVector> Norms;
//    //TArray<int32> Tris;
//    //int32 Vi = 0;
//    //int32 Ni = 0;
//    //int32 Ti = 0;
//    //float Ext = 0.5f * Thickness;
//
//    //// Top
//    //for (int32 i = 0; i < Count; i++)
//    //{
//    //    Verts.Add(FVector(Polygon[i].X, Polygon[i].Y, Ext));
//    //    Norms.Add(FVector(0.f, 0.f, 1.f));
//    //}
//
//    //// Bottom
//    //for (int32 i = 0; i < Count; i++)
//    //{
//    //    Verts.Add(FVector(Polygon[i].X, Polygon[i].Y, -Ext));
//    //    Norms.Add(FVector(0.f, 0.f, -1.f));
//    //}
//
//    //// Sides
//    //for (int32 i = 0; i < Count; i++)
//    //{
//    //    int32 iNext = i == Count - 1 ? 0 : i + 1;
//
//    //    Verts.Add(FVector(Polygon[i].X, Polygon[i].Y, Ext));
//    //    Verts.Add(FVector(Polygon[i].X, Polygon[i].Y, -Ext));
//    //    Verts.Add(FVector(Polygon[iNext].X, Polygon[iNext].Y, -Ext));
//    //    Verts.Add(FVector(Polygon[iNext].X, Polygon[iNext].Y, Ext));
//
//    //    FVector Norm = FVector::CrossProduct(FVector(Polygon[iNext] - Polygon[i], 0.f), FVector(0.f, 0.f, 1.f)).GetSafeNormal();
//
//    //    Norms.Add(Norm);
//    //    Norms.Add(Norm);
//    //    Norms.Add(Norm);
//    //    Norms.Add(Norm);
//    //}
//
//    //for (int32 Vert = 2; Vert < Count; Vert++)
//    //{
//    //    Tris.Add(0);
//    //    Tris.Add(Vert - 1);
//    //    Tris.Add(Vert);
//    //}
//
//    //for (int32 Vert = 2; Vert < Count; Vert++)
//    //{
//    //    Tris.Add(Count);
//    //    Tris.Add(Count + Vert);
//    //    Tris.Add(Count + Vert - 1);
//    //}
//
//    //for (int32 Vert = 0; Vert < Count; Vert++)
//    //{
//    //    int32 Si = 2 * Count + 4 * Vert;
//
//    //    Tris.Add(Si);
//    //    Tris.Add(Si + 1);
//    //    Tris.Add(Si + 2);
//
//    //    Tris.Add(Si);
//    //    Tris.Add(Si + 2);
//    //    Tris.Add(Si + 3);
//    //}
//
//    //check(Ti == Tris.Num());
//    //check(Vi == Verts.Num());
//
//    //UStaticMesh* Mesh = NewObject<UStaticMesh>();
//    ////Mesh->CreateStaticMeshDescription();
//    ////Mesh->GetStaticMeshDescription()->SetVertexPositions(Verts);
//    ////Mesh->GetStaticMeshDescription()->SetNormals(Norms);
//    ////Mesh->GetStaticMeshDescription()->SetTriangles(Tris, false);
//    ////Mesh->CommitMeshDescription();
//
// /*   return Mesh;*/
//}

