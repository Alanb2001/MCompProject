#include "VoronoiClipper.h"
#include "Geom.h"

void FVoronoiClipper::ClipSite(FFVoronoiDiagram& Diag, TArray<FVector2D>& Polygon, int& Site, TArray<FVector2D>& Clipped)
{
    PointsIn.Empty();
    PointsIn.Append(Polygon);

    int FirstEdge, LastEdge;

    if (Site == Diag.Sites->Num() - 1)
    {
        FirstEdge = Diag.FirstEdgeBySite[Site];
        LastEdge = Diag.Edges.Num() - 1;
    }
    else
    {
        FirstEdge = Diag.FirstEdgeBySite[Site];
        LastEdge = Diag.FirstEdgeBySite[Site + 1] - 1;
    }

    for (int Ei = FirstEdge; Ei <= LastEdge; Ei++)
    {
        PointsOut.Empty();

        FFVoronoiDiagram::FFEdge Edge = Diag.Edges[Ei];

        FVector2D Lp, Ld;

        if (Edge.Type == FFVoronoiDiagram::EDgeType::RayCCW || Edge.Type == FFVoronoiDiagram::EDgeType::RayCw)
        {
            Lp = Diag.Vertices[Edge.Vert0];
            Ld = Edge.Direction;

            if (Edge.Type == FFVoronoiDiagram::EDgeType::RayCw)
            {
                Ld *= -1;
            }
        }
        else if (Edge.Type == FFVoronoiDiagram::EDgeType::Segment)
        {
            FVector2D Lp0 = Diag.Vertices[Edge.Vert0];
            FVector2D Lp1 = Diag.Vertices[Edge.Vert1];

            Lp = Lp0;
            Ld = Lp1 - Lp0;
        }
        else
        {
            check(false);
            return;
        }

        for (int PI0 = 0; PI0 < PointsIn.Num(); PI0++)
        {
            int PI1 = PI0 == PointsIn.Num() - 1 ? 0 : PI0 + 1;

            FVector2D P0 = PointsIn[PI0];
            FVector2D P1 = PointsIn[PI1];

            bool bP0Inside = FGeom::ToTheLeft(P0, Lp, Lp + Ld);

            if (bool bP1Inside = FGeom::ToTheLeft(P1, Lp, Lp + Ld); bP0Inside && bP1Inside)
            {
                PointsOut.Add(P1);
            }
            else if (!bP0Inside && !bP1Inside)
            {
                // Do nothing, both are outside
            }
            else
            {
                FVector2D Intersection = FGeom::LineLineIntersection(Lp, Ld.GetSafeNormal(), P0, (P1 - P0).GetSafeNormal());

                if (bP0Inside)
                {
                    PointsOut.Add(Intersection);
                }
                else if (bP1Inside)
                {
                    PointsOut.Add(Intersection);
                    PointsOut.Add(P1);
                }
                else
                {
                    check(false);
                }
            }
        }

        TArray<FVector2D> Tmp = PointsIn;
        PointsIn = PointsOut;
        PointsOut = Tmp;
    }

    if (Clipped.Num() == NULL)
    {
        Clipped = TArray<FVector2D>();
    }
    else
    {
        Clipped.Empty();
    }

    Clipped.Append(PointsIn);
}
