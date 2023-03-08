#include "VoronoiCalculator.h"

VoronoiCalculator::VoronoiCalculator()
{
	delCalc = DelaunayCalculator();
	cmp = PTComparer();
	pts = TArray<PointTriangle>();
}

VoronoiDiagram VoronoiCalculator::CalculateDiagram(const TArray<FVector2D>& inputVertices)
{
	VoronoiDiagram result;
	CalculateDiagram(inputVertices, &result);
	return result;
}

void VoronoiCalculator::CalculateDiagram(const TArray<FVector2D>& inputVertices, VoronoiDiagram* result)
{
	if (inputVertices.Num() < 3)
	{
		throw std::runtime_error("Not implemented for < 3 vertices");
	}

	if (result == nullptr)
	{
		result = new VoronoiDiagram();
	}
	
	DelaunayTriangulation trig = result->Triangulation;

	result->Clear();

	delCalc.CalculateTriangulation(inputVertices, trig);

	pts.Empty();

	auto& verts = trig.Vertices;
	auto& tris = trig.Triangles;
	auto& centers = result->Vertices;
	auto& edges = result->Edges;

	pts.Reserve(tris.Num());
	edges.Reserve(tris.Num());

	if (tris.Num() > pts.Max())
	{
		pts.SetNumUninitialized(tris.Num());
	}
	if (tris.Num() > edges.Max())
	{
		edges.SetNumUninitialized(tris.Num());
	}
	
	for (int ti = 0; ti < tris.Num(); ti += 3)
	{
		auto p0 = verts[tris[ti]];
		auto p1 = verts[tris[ti + 1]];
		auto p2 = verts[tris[ti + 2]];

		assert(Geom::ToTheLeft(p2, p0, p1));

		centers.Add(Geom::CircumcircleCenter(p0, p1, p2));
	}

	for (int ti = 0; ti < tris.Num(); ti += 3)
	{
		pts.Add(PointTriangle(tris[ti], ti));
		pts.Add(PointTriangle(tris[ti + 1], ti));
		pts.Add(PointTriangle(tris[ti + 2], ti));
	}
	
	cmp.tris = tris;
	cmp.verts = verts;
	
	//pts.Sort(cmp);
	
	cmp.tris.Empty();
	cmp.verts.Empty();

	for (int i = 0; i < pts.Num(); i++)
	{
		result->FirstEdgeBySite.Add(edges.Num());

		auto start = i;
		auto end = -1;

		for (int j = i + 1; j < pts.Num(); j++)
		{
			if (pts[i].Point != pts[j].Point) {
				end = j - 1;
				break;
			}
		}

		if (end == -1)
		{
			end = pts.Num() - 1;
		}

		i = end;

		auto count = end - start;

		assert(count >= 0);

		for (int ptiCurr = start; ptiCurr <= end; ptiCurr++)
		{
			bool isEdge;

			auto ptiNext = ptiCurr + 1;

			if (ptiNext > end) ptiNext = start;

			auto ptCurr = pts[ptiCurr];
			auto ptNext = pts[ptiNext];

			auto tiCurr = ptCurr.Triangle;
			auto tiNext = ptNext.Triangle;

			auto p0 = verts[ptCurr.Point];

			FVector2D v2nan = FVector2D(0, 0);

			if (count == 0)
			{
				isEdge = true;
			}
			else if (count == 1)
			{

				FVector2D cCurr = Geom::TriangleCentroid(verts[tris[tiCurr]], verts[tris[tiCurr + 1]], verts[tris[tiCurr + 2]]);
				FVector2D cNext = Geom::TriangleCentroid(verts[tris[tiNext]], verts[tris[tiNext + 1]], verts[tris[tiNext + 2]]);

				isEdge = Geom::ToTheLeft(cCurr, p0, cNext);
			}
			else
			{
				isEdge = !SharesEdge(tris, tiCurr, tiNext);
			}

			if (isEdge)
			{
				FVector2D v0, v1;

				if (ptCurr.Point == tris[tiCurr])
				{
					v0 = verts[tris[tiCurr + 2]] - verts[tris[tiCurr + 0]];
				}
				else if (ptCurr.Point == tris[tiCurr + 1])
				{
					v0 = verts[tris[tiCurr + 0]] - verts[tris[tiCurr + 1]];
				}
				else
				{
					assert(ptCurr.Point == tris[tiCurr + 2]);
					v0 = verts[tris[tiCurr + 1]] - verts[tris[tiCurr + 2]];
				}

				if (ptNext.Point == tris[tiNext])
				{
					v1 = verts[tris[tiNext + 0]] - verts[tris[tiNext + 1]];
				}
				else if (ptNext.Point == tris[tiNext + 1])
				{
					v1 = verts[tris[tiNext + 1]] - verts[tris[tiNext + 2]];
				}
				else
				{
					assert(ptNext.Point == tris[tiNext + 2]);
					v1 = verts[tris[tiNext + 2]] - verts[tris[tiNext + 0]];
				}

				edges.Add(Edge(
					EdgeType::RayCCW,
					ptCurr.Point,
					tiCurr / 3,
					-1,
					Geom::RotateRightAngle(v0)
				));

				edges.Add(Edge(
					EdgeType::RayCW,
					ptCurr.Point,
					tiNext / 3,
					-1,
					Geom::RotateRightAngle(v1)
				));
			}
			else
			{
				if (!Geom::AreCoincident(centers[tiCurr / 3], centers[tiNext / 3]))
				{
					edges.Add(Edge(
						EdgeType::Segment,
						ptCurr.Point,
						tiCurr / 3,
						tiNext / 3,
						v2nan
					));
				}
			}
		}
	}
}

int32 VoronoiCalculator::NonSharedPoint(const TArray<int32>& tris, int32 ti0, int32 ti1)
{
	check(SharesEdge(tris, ti0, ti1));

	const int32 x0 = tris[ti0];
	const int32 x1 = tris[ti0 + 1];
	const int32 x2 = tris[ti0 + 2];

	const int32 y0 = tris[ti1];
	const int32 y1 = tris[ti1 + 1];
	const int32 y2 = tris[ti1 + 2];

	if (x0 != y0 && x0 != y1 && x0 != y2) return x0;
	if (x1 != y0 && x1 != y1 && x1 != y2) return x1;
	if (x2 != y0 && x2 != y1 && x2 != y2) return x2;

	check(false);
	return -1;
}

bool VoronoiCalculator::SharesEdge(const TArray<int32>& tris, int32 ti0, int32 ti1)
{
	int32 x0 = tris[ti0];
	int32 x1 = tris[ti0 + 1];
	int32 x2 = tris[ti0 + 2];

	int32 y0 = tris[ti1];
	int32 y1 = tris[ti1 + 1];
	int32 y2 = tris[ti1 + 2];

	int32 n = 0;

	if (x0 == y0 || x0 == y1 || x0 == y2) {
		n++;
	}
	if (x1 == y0 || x1 == y1 || x1 == y2) {
		n++;
	}
	if (x2 == y0 || x2 == y1 || x2 == y2) {
		n++;
	}

	check(n != 3);

	return n >= 2;
}
