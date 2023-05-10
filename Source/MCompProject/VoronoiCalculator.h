#pragma once

#include "CoreMinimal.h"
#include "DelaunayCalculator.h"
#include "FFVoronoiDiagram.h"
#include "Geom.h"

class FVoronoiCalculator
{
	struct FPointTriangle
	{
		int Point;
		int Triangle;

		FPointTriangle(int PointP, int TriangleP)
			: Point(PointP), Triangle(TriangleP)
		{
		}
	};

	class FPTComparer
	{
	public:
		TArray<FVector2D> verts;
		TArray<int32> tris;
		
		bool operator ()(const FPointTriangle& Pt0, const FPointTriangle& Pt1) const
		{
			return static_cast<bool>(Compare(Pt0, Pt1));
		}
		
		int Compare(FPointTriangle Pt0, FPointTriangle Pt1) const
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
		
		int CompareAngles(FPointTriangle pt0, FPointTriangle pt1) const
		{
			check(pt0.Point == pt1.Point);

			UE::Math::TVector2<double> rp = verts[pt0.Point];
			
			FVector2D p0 = Centroid(pt0) - rp;
			FVector2D p1 = Centroid(pt1) - rp;
			
			bool q0 = p0.Y < 0 || (p0.Y == 0 && p0.X < 0);
			bool q1 = p1.Y < 0 || (p1.Y == 0 && p1.X < 0);

			if (q0 == q1)
			{
				float cp = p0.X * p1.Y - p0.Y * p1.X;

				if (cp > 0)
				{
					return -1;
				}
				if (cp < 0)
				{
					return 1;
				}
				return 0;
			}
			
			return q1 ? -1 : 1;
		}
		
		FVector2D Centroid(FPointTriangle pt) const
		{
			int ti = pt.Triangle;
			return FGeom::TriangleCentroid(verts[tris[ti]], verts[tris[ti + 1]], verts[tris[ti + 2]]);
		}
	};
public:
	FPTComparer Cmp;
	FDelaunayCalculator DelCalc;
	TArray<FPointTriangle> Pts;
	
	FVoronoiCalculator();

	FFVoronoiDiagram CalculateDiagram(TArray<FVector2D> InputVertices);

	void CalculateDiagram(TArray<FVector2D> InputVertices, FFVoronoiDiagram& Result);

	static bool SharesEdge(TArray<int> Tris, int Ti0, int Ti1);
};