#include "DelaunayTriangulation.h"
#include "Geom.h"

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

bool FDelaunayTriangulation::Verify() const
{
    try
    {
        for (int i = 0; i < Triangles.Num(); i += 3)
        {
            FVector2D C0 = Vertices[Triangles[i]];
            FVector2D C1 = Vertices[Triangles[i + 1]];
            FVector2D C2 = Vertices[Triangles[i + 2]];

            for (int j = 0; j < Vertices.Num(); j++)
            {
                if (FVector2D p = Vertices[j]; FGeom::InsideCircumcircle(p, C0, C1, C2)) {
                    return false;
                }
            }
        }

        return true;
    }
    catch (...)
    {
        return false;
    }
}
