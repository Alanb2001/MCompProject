#pragma once

#include "CoreMinimal.h"
#include "DelaunayCalculator.h"
#include "FFVoronoiDiagram.h"
#include "Geom.h"

template <class T>
struct Comparer
{
	Comparer(void){}
	~Comparer(){}

	int operator () (const T & a, const T & b) const
	{
		return ( (a < b) ? -1 : ( (a > b) ? 1 : 0) );
	}

	int operator () (const T * a, const T * b) const
	{
		return ( (*a < *b) ? -1 : ( (*a > *b) ? 1 : 0) );
	}
};

template <>
struct Comparer <std::string>
{
	Comparer(void){}
	~Comparer(){}

	int operator () (std::string & a, std::string & b)
	{
		return a.compare(b);
	}
};

class FVoronoiCalculator
{
		struct FPointTriangle
	{
		const int Point;
		const int Triangle;

		FPointTriangle(const int PointP, const int TriangleP)
			: Point(PointP), Triangle(TriangleP)
		{
		}

		FPointTriangle(TArray<int> Tri, int Ti);
	};

	class FPTComparer /*: public Comparer<FPointTriangle>*/
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
			else if (Pt0.Point > Pt1.Point)
			{
				return 1;
			}
			else if (Pt0.Triangle == Pt1.Triangle)
			{
				check(Pt0.Point == Pt1.Point);
				return 0;
			}
			else
			{
				return CompareAngles(Pt0, Pt1);
			}
		}
		
		int CompareAngles(FPointTriangle pt0, FPointTriangle pt1) const
		{
			check(pt0.Point == pt1.Point);

			// "reference" point
			auto rp = verts[pt0.Point];

			// triangle centroids in "reference point space"
			FVector2D p0 = Centroid(pt0) - rp;
			FVector2D p1 = Centroid(pt1) - rp;

			// quadrants. false for 1,2, true for 3,4.
			bool q0 = ((p0.Y < 0) || ((p0.Y == 0) && (p0.X < 0)));
			bool q1 = ((p1.Y < 0) || ((p1.Y == 0) && (p1.X < 0)));

			if (q0 == q1)
			{
				// p0 and p1 are within 180 degrees of each other, so just
				// use cross product to find out if pt1 is to the left of
				// p0.
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
				// if q0 != q1, q1 is true, then p0 is in quadrants 1 or 2,
				// and p1 is in quadrants 3 or 4. Hence, pt0 < pt1. If q1
				// is not true, vice versa.
				return q1 ? -1 : 1;
			}
		}
		
		FVector2D Centroid(FPointTriangle pt) const
		{
			auto ti = pt.Triangle;
			return FGeom::TriangleCentroid(verts[tris[ti]], verts[tris[ti + 1]], verts[tris[ti + 2]]);
		}
		
		//bool Compare(const FPointTriangle& pt0, const FPointTriangle& pt1) 
		//{
		//	if (pt0.Point < pt1.Point)
		//	{
		//		return true;
		//	}
		//	else if (pt0.Point > pt1.Point)
		//	{
		//		return false;
		//	}
		//	else if (pt0.Triangle == pt1.Triangle)
		//	{
		//		check(pt0.Point == pt1.Point);
		//		return true;
		//	}
		//	else
		//	{
		//		return CompareAngles(pt0, pt1);
		//	}
		//}
		

		
		//TFunction<bool(const FPointTriangle&, const FPointTriangle&)> Cmp =
		//	[this](const FPointTriangle& Pt0, const FPointTriangle& Pt1) -> bool
		//{
		//	if (Pt0.Point < Pt1.Point)
		//	{
		//		return true;
		//	}
		//	else if (Pt0.Point > Pt1.Point)
		//	{
		//		return false;
		//	}
		//	else if (Pt0.Triangle == Pt1.Triangle)
		//	{
		//		check(Pt0.Point == Pt1.Point);
		//		return true;
		//	}
		//	else
		//	{
		//		return CompareAngles(Pt0, Pt1) < 0;
		//	}
		//};
	};
public:
	FPTComparer Cmp;
	FDelaunayCalculator DelCalc;
	TArray<FPointTriangle> Pts;
	
	FVoronoiCalculator();

	FFVoronoiDiagram CalculateDiagram(TArray<FVector2D>& InputVertices);

	void CalculateDiagram(TArray<FVector2D>& InputVertices, FFVoronoiDiagram* Result);

	static bool SharesEdge(const TArray<int> Tris, const int Ti0, const int Ti1);
};