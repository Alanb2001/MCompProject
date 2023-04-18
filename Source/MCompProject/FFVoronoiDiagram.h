#pragma once

#include "CoreMinimal.h"
#include "DelaunayTriangulation.h"

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

    FString ToString() const
    {
        FString OutputSting;
    
        if (Type == EDgeType::Segment)
        {
            OutputSting = FString::Printf(TEXT("VoronoiEdge(Segment, %d, %d, %d)"), Site, Vert0, Vert1);
        }
        else if (Type == EDgeType::Line)
        {
            OutputSting = FString::Printf(TEXT("VoronoiEdge(Line, %d, %d, (%f, %f))"), Site, Vert0, Direction.X, Direction.Y);
        }
        else if (Type == EDgeType::RayCCW || Type == EDgeType::RayCw)
        {
            OutputSting = FString::Printf(TEXT("VoronoiEdge(Ray, %d, %d, (%f, %f))"), Site, Vert0, Direction.X, Direction.Y);
        }
        else
        {
            OutputSting =  FString(TEXT("Invalid VoronoiEdge"));
        }

        UE_LOG(LogTemp, Warning, TEXT("%s"), *OutputSting);

        return OutputSting;
    }
};

class FFVoronoiDiagram
{
public:
    FDelaunayTriangulation* Triangulation;
    TArray<FVector2D>* Sites;
    TArray<FVector2D> Vertices;
    TArray<FFEdge> Edges;
    TArray<int> FirstEdgeBySite;

    FFVoronoiDiagram();

    void Clear();
};
