#include "VoronoiCalculator.h"
#include "Geom.h"

FVoronoiCalculator::FVoronoiCalculator()
{
	DelCalc = FDelaunayCalculator();
	Cmp = FPtComparer();
	Pts = TArray<FPointTriangle>();
}

FFVoronoiDiagram FVoronoiCalculator::CalculateDiagram(const TArray<FVector2D>& InputVertices)
{
	FFVoronoiDiagram Result;
	CalculateDiagram(InputVertices, &Result);
	return Result;
}

void FVoronoiCalculator::CalculateDiagram(const TArray<FVector2D>& InputVertices, FFVoronoiDiagram* Result)
{
	if (InputVertices.Num() < 3)
	{
		throw std::runtime_error("Not implemented for < 3 vertices");
	}

	if (Result == nullptr)
	{
		Result = new FFVoronoiDiagram();
	}
	
	FDelaunayTriangulation trig = Result->Triangulation;

	Result->Clear();

	DelCalc.CalculateTriangulation(InputVertices, &trig);

	Pts.Empty();

	auto& Verts = trig.Vertices;
	auto& Tris = trig.Triangles;
	auto& Centers = Result->Vertices;
	auto& Edges = Result->Edges;

	Pts.Reserve(Tris.Num());
	Edges.Reserve(Tris.Num());

	if (Tris.Num() > Pts.Max())
	{
		Pts.SetNumUninitialized(Tris.Num());
	}
	if (Tris.Num() > Edges.Max())
	{
		Edges.SetNumUninitialized(Tris.Num());
	}
	
	for (int Ti = 0; Ti < Tris.Num(); Ti += 3)
	{
		auto P0 = Verts[Tris[Ti]];
		auto P1 = Verts[Tris[Ti + 1]];
		auto P2 = Verts[Tris[Ti + 2]];

		check(FGeom::ToTheLeft(P2, P0, P1));

		Centers.Add(FGeom::CircumcircleCenter(P0, P1, P2));
	}

	for (int Ti = 0; Ti < Tris.Num(); Ti += 3)
	{
		Pts.Add(FPointTriangle(Tris[Ti], Ti));
		Pts.Add(FPointTriangle(Tris[Ti + 1], Ti));
		Pts.Add(FPointTriangle(Tris[Ti + 2], Ti));
	}
	
	Cmp.Tris = Tris;
	Cmp.Verts = Verts;
	
	//pts.Sort(cmp);
	
	Cmp.Tris.Empty();
	Cmp.Verts.Empty();

	for (int i = 0; i < Pts.Num(); i++)
	{
		Result->FirstEdgeBySite.Add(Edges.Num());

		const int Start = i;
		int End = -1;

		for (int j = i + 1; j < Pts.Num(); j++)
		{
			if (Pts[i].Point != Pts[j].Point)
			{
				End = j - 1;
				break;
			}
		}

		if (End == -1)
		{
			End = Pts.Num() - 1;
		}

		i = End;

		const int Count = End - Start;

		check(Count >= 0);

		for (int PtiCurr = Start; PtiCurr <= End; PtiCurr++)
		{
			bool bIsEdge;

			int PtiNext = PtiCurr + 1;

			if (PtiNext > End)
			{
				PtiNext = Start;
			}

			const FPointTriangle PtCurr = Pts[PtiCurr];
			const FPointTriangle PtNext = Pts[PtiNext];

			const int TiCurr = PtCurr.Triangle;
			const int TiNext = PtNext.Triangle;

			auto p0 = Verts[PtCurr.Point];

			const FVector2D V2NAN = FVector2D(0, 0);

			if (Count == 0)
			{
				bIsEdge = true;
			}
			else if (Count == 1)
			{
				FVector2D CCurr = FGeom::TriangleCentroid(Verts[Tris[TiCurr]], Verts[Tris[TiCurr + 1]], Verts[Tris[TiCurr + 2]]);
				FVector2D CNext = FGeom::TriangleCentroid(Verts[Tris[TiNext]], Verts[Tris[TiNext + 1]], Verts[Tris[TiNext + 2]]);

				bIsEdge = FGeom::ToTheLeft(CCurr, p0, CNext);
			}
			else
			{
				bIsEdge = !SharesEdge(Tris, TiCurr, TiNext);
			}

			if (bIsEdge)
			{
				FVector2D V0, V1;

				if (PtCurr.Point == Tris[TiCurr])
				{
					V0 = Verts[Tris[TiCurr + 2]] - Verts[Tris[TiCurr + 0]];
				}
				else if (PtCurr.Point == Tris[TiCurr + 1])
				{
					V0 = Verts[Tris[TiCurr + 0]] - Verts[Tris[TiCurr + 1]];
				}
				else
				{
					check(PtCurr.Point == Tris[TiCurr + 2]);
					V0 = Verts[Tris[TiCurr + 1]] - Verts[Tris[TiCurr + 2]];
				}

				if (PtNext.Point == Tris[TiNext])
				{
					V1 = Verts[Tris[TiNext + 0]] - Verts[Tris[TiNext + 1]];
				}
				else if (PtNext.Point == Tris[TiNext + 1])
				{
					V1 = Verts[Tris[TiNext + 1]] - Verts[Tris[TiNext + 2]];
				}
				else
				{
					check(PtNext.Point == Tris[TiNext + 2]);
					V1 = Verts[Tris[TiNext + 2]] - Verts[Tris[TiNext + 0]];
				}

				Edges.Add(FFEdge(
					EDgeType::RayCCW,
					PtCurr.Point,
					TiCurr / 3,
					-1,
					FGeom::RotateRightAngle(V0)
				));

				Edges.Add(FFEdge(
					EDgeType::RayCw,
					PtCurr.Point,
					TiNext / 3,
					-1,
					FGeom::RotateRightAngle(V1)
				));
			}
			else
			{
				if (!FGeom::AreCoincident(Centers[TiCurr / 3], Centers[TiNext / 3]))
				{
					Edges.Add(FFEdge(
						EDgeType::Segment,
						PtCurr.Point,
						TiCurr / 3,
						TiNext / 3,
						V2NAN
					));
				}
			}
		}
	}
}

int32 FVoronoiCalculator::NonSharedPoint(const TArray<int>& Tris, const int Ti0, const int Ti1)
{
	check(SharesEdge(Tris, Ti0, Ti1));

	const int X0 = Tris[Ti0];
	const int X1 = Tris[Ti0 + 1];
	const int X2 = Tris[Ti0 + 2];

	const int Y0 = Tris[Ti1];
	const int Y1 = Tris[Ti1 + 1];
	const int Y2 = Tris[Ti1 + 2];

	if (X0 != Y0 && X0 != Y1 && X0 != Y2) return X0;
	if (X1 != Y0 && X1 != Y1 && X1 != Y2) return X1;
	if (X2 != Y0 && X2 != Y1 && X2 != Y2) return X2;

	check(false);
	return -1;
}

bool FVoronoiCalculator::SharesEdge(const TArray<int>& Tris, const int Ti0, const int Ti1)
{
	const int X0 = Tris[Ti0];
	const int X1 = Tris[Ti0 + 1];
	const int X2 = Tris[Ti0 + 2];

	const int Y0 = Tris[Ti1];
	const int Y1 = Tris[Ti1 + 1];
	const int Y2 = Tris[Ti1 + 2];

	int n = 0;

	if (X0 == Y0 || X0 == Y1 || X0 == Y2)
	{
		n++;
	}
	if (X1 == Y0 || X1 == Y1 || X1 == Y2)
	{
		n++;
	}
	if (X2 == Y0 || X2 == Y1 || X2 == Y2)
	{
		n++;
	}

	check(n != 3);

	return n >= 2;
}
