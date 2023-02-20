// Fill out your copyright notice in the Description page of Project Settings.

#include "DelaunayUsage.h"
#include "Delaunay.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ADelaunayUsage::ADelaunayUsage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADelaunayUsage::BeginPlay()
{
	Super::BeginPlay();

	FDelaunay Delaunay;

	TArray<FDPoint> Points;
	Points.Add({ 10.f, 20.f, -1 });
	Points.Add({ 15.f, 3.f, -1 });
	Points.Add({ 6.f, 12.f, -1 });
	Points.Add({ 30.f, 40.f, -1 });
	Points.Add({ 25.f, 17.f, -1 });
	const TArray<FDTriangle> Triangles = Delaunay.Triangulate(Points, 1);

	for (const FDTriangle& TriangleX : Triangles) {
		for (int32 i = 0; i < 3; i++) {
			const FDEdge EdgeX = (
				i == 0 ? TriangleX.E1
				: i == 1 ? TriangleX.E2
				: i == 2 ? TriangleX.E3
				// Invalid
				: FDEdge(FDPoint(0.f, 0.f, -1), FDPoint(0.f, 0.f, -1))
				);
			// For each edge per triangle per room:

			UKismetSystemLibrary::DrawDebugLine(
				this
				, FVector(EdgeX.P1.X, EdgeX.P1.Y, 0.f)
				, FVector(EdgeX.P2.X, EdgeX.P2.Y, 0.f)
				, FColor::Silver
				, 5.f
				, 8.f
			);
		}
	}
}

// Called every frame
void ADelaunayUsage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
