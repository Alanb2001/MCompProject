#include "DelaunayTriangulation.h"

DelaunayTriangulation::DelaunayTriangulation()
{
    Vertices = TArray<FVector2D>();
    Triangles = TArray<int32>();
}

void DelaunayTriangulation::Clear()
{
    Vertices.Empty();
    Triangles.Empty();
}

bool DelaunayTriangulation::Verify() const
{
    try
    {
        for (int32 i = 0; i < Triangles.Num(); i += 3)
        {
            FVector2D c0 = Vertices[Triangles[i]];
            FVector2D c1 = Vertices[Triangles[i + 1]];
            FVector2D c2 = Vertices[Triangles[i + 2]];

            for (int32 j = 0; j < Vertices.Num(); j++)
            {
                FVector2D p = Vertices[j];
                if (Geom::InsideCircumcircle(p, c0, c1, c2)) {
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
