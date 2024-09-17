#include "FFVoronoiDiagram.h"

FFVoronoiDiagram::FFVoronoiDiagram()
{
    Triangulation = FDelaunayTriangulation();
    Sites = &Triangulation.Vertices;
    Vertices = TArray<FVector2D>();
    Edges = TArray<FFEdge>();
    FirstEdgeBySite = TArray<int>();
}

void FFVoronoiDiagram::Clear()
{
    Triangulation.Clear();
    Sites->Empty();
    Vertices.Empty();
    Edges.Empty();
    FirstEdgeBySite.Empty();
}
