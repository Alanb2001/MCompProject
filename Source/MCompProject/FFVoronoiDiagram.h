#pragma once

#include "CoreMinimal.h"
#include "DelaunayTriangulation.h"

class FFVoronoiDiagram
{
public:
    enum class EDgeType
    {
        Line,
        RayCCW,
        RayCw,
        Segment
    };

    struct FFEdge
    {
        EDgeType Type;
        int Site;
        int Vert0;
        int Vert1;
        FVector2D Direction;

        FFEdge(const EDgeType InType, const int InSite, const int InVert0, const int InVert1, const FVector2D InDirection)
            : Type(InType), Site(InSite), Vert0(InVert0), Vert1(InVert1), Direction(InDirection) {}
    };
    FDelaunayTriangulation* Triangulation;
    TArray<FVector2D>* Sites;
    TArray<FVector2D> Vertices;
    TArray<FFEdge> Edges;
    TArray<int> FirstEdgeBySite;

    FFVoronoiDiagram();

    void Clear();
};
