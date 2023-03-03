#pragma once

#include "CoreMinimal.h"
#include "DelaunayCalculator.h"
#include "VoronoiDiagram.h"
#include <cassert>

struct PointTriangle
{
	const int Point;
	const int Triangle;

	PointTriangle(int point, int triangle)
		: Point(point), Triangle(triangle)
	{}

	FString ToString() const
	{
		return FString::Printf(TEXT("PointTriangle(%d, %d)"), Point, Triangle);
	}
};

class PTComparer 
{
public:
	std::vector<FVector2D> verts;
	std::vector<int32> tris;

	int32 Compare(const PointTriangle& pt0, const PointTriangle& pt1) const
	{
		if (pt0.Point < pt1.Point)
		{
			return -1;
		}
		else if (pt0.Point > pt1.Point)
		{
			return 1;
		}
		else if (pt0.Triangle == pt1.Triangle)
		{
			assert(pt0.Point == pt1.Point);
			return 0;
		}
		else
		{
			return CompareAngles(pt0, pt1);
		}
	}

	int32 CompareAngles(const PointTriangle& pt0, const PointTriangle& pt1) const
	{
		assert(pt0.Point == pt1.Point);

		FVector2D rp = verts[pt0.Point];

		FVector2D p0 = Centroid(pt0) - rp;
		FVector2D p1 = Centroid(pt1) - rp;

		bool q0 = ((p0.Y < 0) || ((p0.Y == 0) && (p0.X < 0)));
		bool q1 = ((p1.Y < 0) || ((p1.Y == 0) && (p1.Y < 0)));

		if (q0 == q1)
		{
			float cp = p0.X * p1.Y - p0.Y * p1.X;

			if (cp > 0)
			{
				return -1;
			}
			else if (cp < 0)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return q1 ? -1 : 1;
		}
	}

	FVector2D Centroid(const PointTriangle& pt) const
	{
		int32 ti = pt.Triangle;
		return Geom::TriangleCentroid(verts[tris[ti]], verts[tris[ti + 1]], verts[tris[ti + 2]]);
	}
};

class VoronoiCalculator
{
public:
	DelaunayCalculator delCalc;
	PTComparer cmp;
	TArray<PointTriangle> pts;

	VoronoiCalculator();

	VoronoiDiagram CalculateDiagram(const TArray<FVector2D>& inputVertices);

	void CalculateDiagram(const TArray<FVector2D>& inputVertices, VoronoiDiagram& result);

	static int32 NonSharedPoint(const TArray<int32>& tris, int32 ti0, int32 ti1);

	static bool SharesEdge(const TArray<int32>& tris, int32 ti0, int32 ti1);
};
