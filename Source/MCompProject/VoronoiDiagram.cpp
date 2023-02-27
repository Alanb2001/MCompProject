// Fill out your copyright notice in the Description page of Project Settings.


#include "VoronoiDiagram.h"

VoronoiDiagram::VoronoiDiagram()
{
    Triangulation = DelaunayTriangulation();
    Sites = Triangulation.Vertices;
    Vertices = TArray<FVector2D>();
    Edges = TArray<Edge>();
    FirstEdgeBySite = TArray<int32>();
}

void VoronoiDiagram::Clear()
{
    Triangulation.Clear();
    Sites.Empty();
    Vertices.Empty();
    Edges.Empty();
    FirstEdgeBySite.Empty();
}
