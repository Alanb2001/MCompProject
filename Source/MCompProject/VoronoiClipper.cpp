#include "VoronoiClipper.h"

void VoronoiClipper::ClipSite(VoronoiDiagram diag, TArray<FVector2D> polygon, int site, TArray<FVector2D>& clipped)
{
    pointsIn.Empty();
    pointsIn.Append(polygon);

    int firstEdge, lastEdge;

    if (site == diag.Sites.Num() - 1)
    {
        firstEdge = diag.FirstEdgeBySite[site];
        lastEdge = diag.Edges.Num() - 1;
    }
    else
    {
        firstEdge = diag.FirstEdgeBySite[site];
        lastEdge = diag.FirstEdgeBySite[site + 1] - 1;
    }

    for (int ei = firstEdge; ei <= lastEdge; ei++)
    {
        pointsOut.Empty();

        Edge edge = diag.Edges[ei];

        FVector2D lp, ld;

        if (edge.Type == EdgeType::RayCCW || edge.Type == EdgeType::RayCW)
        {
            lp = diag.Vertices[edge.Vert0];
            ld = edge.Direction;

            if (edge.Type == EdgeType::RayCW)
            {
                ld *= -1;
            }
        }
        else if (edge.Type == EdgeType::Segment)
        {
            FVector2D lp0 = diag.Vertices[edge.Vert0];
            FVector2D lp1 = diag.Vertices[edge.Vert1];

            lp = lp0;
            ld = lp1 - lp0;
        }
        else if (edge.Type == EdgeType::Line)
        {
            throw new std::exception("Haven't implemented voronoi halfplanes yet");
        }
        else
        {
            check(false);
            return;
        }

        for (int pi0 = 0; pi0 < pointsIn.Num(); pi0++)
        {
            int pi1 = pi0 == pointsIn.Num() - 1 ? 0 : pi0 + 1;

            FVector2D p0 = pointsIn[pi0];
            FVector2D p1 = pointsIn[pi1];

            bool p0Inside = Geom::ToTheLeft(p0, lp, lp + ld);
            bool p1Inside = Geom::ToTheLeft(p1, lp, lp + ld);

            if (p0Inside && p1Inside)
            {
                pointsOut.Add(p1);
            }
            else if (!p0Inside && !p1Inside)
            {
                // Do nothing, both are outside
            }
            else
            {
                FVector2D intersection = Geom::LineLineIntersection(lp, ld.GetSafeNormal(), p0, (p1 - p0).GetSafeNormal());

                if (p0Inside)
                {
                    pointsOut.Add(intersection);
                }
                else if (p1Inside)
                {
                    pointsOut.Add(intersection);
                    pointsOut.Add(p1);
                }
                else
                {
                    check(false);
                }
            }
        }

        TArray<FVector2D> tmp = pointsIn;
        pointsIn = pointsOut;
        pointsOut = tmp;
    }

    if (clipped.Num() == 0)
    {
        clipped = pointsIn;
    }
    else
    {
        clipped.Empty();
        clipped.Append(pointsIn);
    }
}
