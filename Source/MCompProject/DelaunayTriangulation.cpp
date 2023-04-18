#include "DelaunayTriangulation.h"

FDelaunayTriangulation::FDelaunayTriangulation()
{
    Vertices = TArray<FVector2D>();
    Triangles = TArray<int>();
}

void FDelaunayTriangulation::Clear()
{
    Vertices.Empty();
    Triangles.Empty();
}
