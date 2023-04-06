#pragma once

#include "CoreMinimal.h"
#include "DelaunayCalculator.h"
#include "FFVoronoiDiagram.h"
#include "Geom.h"

struct FPointTriangle
{
	const int Point;
	const int Triangle;

	FPointTriangle(const int PointP, const int TriangleP)
		: Point(PointP), Triangle(TriangleP)
	{}

	FString ToString() const
	{
		return FString::Printf(TEXT("PointTriangle(%d, %d)"), Point, Triangle);
	}
};

class FPtComparer 
{
public:
	TArray<FVector2D> Verts;
	TArray<int> Tris;

	int Compare(const FPointTriangle& Pt0, const FPointTriangle& Pt1) const
	{
		if (Pt0.Point < Pt1.Point)
		{
			return -1;
		}
		if (Pt0.Point > Pt1.Point)
		{
			return 1;
		}
		if (Pt0.Triangle == Pt1.Triangle)
		{
			check(Pt0.Point == Pt1.Point);
			return 0;
		}
		
		return CompareAngles(Pt0, Pt1);
	}

	int CompareAngles(const FPointTriangle& Pt0, const FPointTriangle& Pt1) const
	{
		check(Pt0.Point == Pt1.Point);

		const FVector2D Rp = Verts[Pt0.Point];

		const FVector2D P0 = Centroid(Pt0) - Rp;
		const FVector2D P1 = Centroid(Pt1) - Rp;

		const bool bQ0 = P0.Y < 0 || P0.Y == 0 && P0.X < 0;

		if (const bool bQ1 = P1.Y < 0 || P1.Y == 0 && P1.Y < 0; bQ0 == bQ1)
		{
			if (const float CP = P0.X * P1.Y - P0.Y * P1.X; CP > 0)
			{
				return -1;
			}
			else
			{
				if (CP < 0)
				{
					return 1;
				}
				
				return 0;
			}
		}
		else
		{
			return bQ1 ? -1 : 1;
		}
	}

	FVector2D Centroid(const FPointTriangle& Pt) const
	{
		const int Ti = Pt.Triangle;
		return FGeom::TriangleCentroid(Verts[Tris[Ti]], Verts[Tris[Ti + 1]], Verts[Tris[Ti + 2]]);
	}
};

class FVoronoiCalculator
{
public:
	FDelaunayCalculator DelCalc;
	FPtComparer Cmp;
	TArray<FPointTriangle> Pts;

	FVoronoiCalculator();

	FFVoronoiDiagram CalculateDiagram(const TArray<FVector2D>& InputVertices);

	void CalculateDiagram(const TArray<FVector2D>& InputVertices, FFVoronoiDiagram* Result);

	static int32 NonSharedPoint(const TArray<int>& Tris, const int Ti0, const int Ti1);

	static bool SharesEdge(const TArray<int>& Tris, const int Ti0, const int Ti1);
};
