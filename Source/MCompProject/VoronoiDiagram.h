#pragma once

#include "CoreMinimal.h"
#include "DelaunayTriangulation.h"

enum class EdgeType
{
    Line,
    RayCCW,
    RayCW,
    Segment
};

struct Edge
{
    EdgeType Type;
    int Site;
    int Vert0;
    int Vert1;
    FVector2D Direction;

    Edge(EdgeType InType, int InSite, int InVert0, int InVert1, FVector2D InDirection)
        : Type(InType), Site(InSite), Vert0(InVert0), Vert1(InVert1), Direction(InDirection) {}

    FString ToString() const
    {
        if (Type == EdgeType::Segment)
        {
            return FString::Printf(TEXT("VoronoiEdge(Segment, %d, %d, %d)"), Site, Vert0, Vert1);
        }
        else if (Type == EdgeType::Line)
        {
            return FString::Printf(TEXT("VoronoiEdge(Line, %d, %d, (%f, %f))"), Site, Vert0, Direction.X, Direction.Y);
        }
        else if (Type == EdgeType::RayCCW || Type == EdgeType::RayCW)
        {
            return FString::Printf(TEXT("VoronoiEdge(Ray, %d, %d, (%f, %f))"), Site, Vert0, Direction.X, Direction.Y);
        }
        else
        {
            return FString(TEXT("Invalid VoronoiEdge"));
        }
    }
};

class VoronoiDiagram
{
public:
    DelaunayTriangulation Triangulation;
    TArray<FVector2D> Sites;
    TArray<FVector2D> Vertices;
    TArray<Edge> Edges;
    TArray<int32> FirstEdgeBySite;

    VoronoiDiagram();

    void Clear();
};
