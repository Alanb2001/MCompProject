#include "DelaunayCalculator.h"
#include "Geom.h"

FDelaunayCalculator::FDelaunayCalculator()
{
	Triangles = TArray<FTriangleNode>();
}

FDelaunayTriangulation* FDelaunayCalculator::CalculateTriangulation(const TArray<FVector2D>& Verts1)
{
	FDelaunayTriangulation* Result = nullptr;

	CalculateTriangulation(Verts1, Result);

	return Result;
}

void FDelaunayCalculator::CalculateTriangulation(const TArray<FVector2D>& Verts1, FDelaunayTriangulation* Result)
{
	if (Verts1.Num() == 0) 
	{
		throw std::invalid_argument("Points");
	}
	
	if (Verts1.Num() < 3)
	{
		throw std::invalid_argument("You need at least 3 points for a triangulation");
	}

	Triangles.Empty();
	this->Verts = Verts1;

	Highest = 0;

	for (int i = 0; i < Verts1.Num(); i++)
	{
		if (Higher(Highest, i))
		{
			Highest = i;
		}
	}

	Triangles.Add(FTriangleNode(-2, -1, Highest));

	RunBowyerWatson();
	GenerateResult(Result);

	this->Verts.Empty();
}

bool FDelaunayCalculator::Higher(const int PI0, const int PI1)
{
	if (PI0 == -2)
	{
		return false;
	}
	if (PI0 == -1)
	{
		return true;
	}
	if (PI1 == -2)
	{
		return true;
	}
	if (PI1 == -1)
	{
		return false;
	}

	const FVector2D P0 = Verts[PI0];
	const FVector2D P1 = Verts[PI1];

	if (P0.Y < P1.Y)
	{
		return true;
	}
	if (P0.Y > P1.Y)
	{
		return false;
	}
	return P0.X < P1.X;
}

void FDelaunayCalculator::RunBowyerWatson()
{
	for (int i = 0; i < Verts.Num(); i++)
	{
		const int PiP = i;
		
		if (PiP == Highest)
		{
			continue;
		}

		const int Ti = FindTriangleNode(PiP);
		
		FTriangleNode t = Triangles[Ti];

		const int P0 = t.P0;
		const int P1 = t.P1;
		const int P2 = t.P2;

		const int Nti0 = Triangles.Num();
		const int Nti1 = Nti0 + 1;
		const int Nti2 = Nti0 + 2;

		FTriangleNode Nt0 = FTriangleNode(PiP, P0, P1);
		FTriangleNode Nt1 = FTriangleNode(PiP, P1, P2);
		FTriangleNode Nt2 = FTriangleNode(PiP, P2, P0);

		Nt0.A0 = t.A2;
		Nt1.A0 = t.A0;
		Nt2.A0 = t.A1;

		Nt0.A1 = Nti1;
		Nt1.A1 = Nti2;
		Nt2.A1 = Nti0;

		Nt0.A2 = Nti2;
		Nt1.A2 = Nti0;
		Nt2.A2 = Nti1;

		t.C0 = Nti0;
		t.C1 = Nti1;
		t.C2 = Nti2;

		Triangles[Ti] = t;

		Triangles.Add(Nt0);
		Triangles.Add(Nt1);
		Triangles.Add(Nt2);

		if (Nt0.A0 != -1) LegalizeEdge(Nti0, Nt0.A0, PiP, P0, P1);
		if (Nt1.A0 != -1) LegalizeEdge(Nti1, Nt1.A0, PiP, P1, P2);
		if (Nt2.A0 != -1) LegalizeEdge(Nti2, Nt2.A0, PiP, P2, P0);
	}
}

void FDelaunayCalculator::GenerateResult(FDelaunayTriangulation* Result)
{
	if (Result == nullptr)
	{
		Result = new FDelaunayTriangulation();
	}

	Result->Clear();

	for (int i = 0; i < Verts.Num(); i++)
	{
		Result->Vertices.Add(Verts[i]);
	}

	for (int i = 1; i < Triangles.Num(); i++)
	{
		if (const FTriangleNode& t = Triangles[i]; t.IsLeaf() && t.IsInner())
		{
			Result->Triangles.Add(t.P0);
			Result->Triangles.Add(t.P1);
			Result->Triangles.Add(t.P2);
		}
	}
}

int FDelaunayCalculator::LeafWithEdge(int Ti, const int E0, const int E1)
{
	check(Triangles[Ti].HasEdge(E0, E1));

	while (!Triangles[Ti].IsLeaf())
	{
		if (const FTriangleNode t = Triangles[Ti]; t.C0 != -1 && Triangles[t.C0].HasEdge(E0, E1))
		{
			Ti = t.C0;
		}
		else if (t.C1 != -1 && Triangles[t.C1].HasEdge(E0, E1))
		{
			Ti = t.C1;
		}
		else if (t.C2 != -1 && Triangles[t.C2].HasEdge(E0, E1))
		{
			Ti = t.C2;
		}
		else
		{
			check(false);
			throw std::exception("This should never happen");
		}
	}

	return Ti;
}

bool FDelaunayCalculator::LegalEdge(const int K, const int L, const int I, const int J)
{
	checkf(K != Highest && K >= 0, TEXT("Assertion failed: k != highest && k >= 0"));

	const bool bLMagic = L < 0;
	const bool bIMagic = I < 0;
	const bool bJMagic = J < 0;

	checkf(!(bIMagic && bJMagic), TEXT("Assertion failed: !(iMagic && jMagic)"));

	if (bLMagic)
	{
		return true;
	}
	if (bIMagic)
	{
		checkf(!bJMagic, TEXT("Assertion failed: !jMagic"));

		const FVector2D& p = Verts[L];
		const FVector2D& L0 = Verts[K];
		const FVector2D& L1 = Verts[J];

		return FGeom::ToTheLeft(p, L0, L1);
	}
	if (bJMagic)
	{
		checkf(!bIMagic, TEXT("Assertion failed: !iMagic"));

		const FVector2D& p = Verts[L];
		const FVector2D& L0 = Verts[K];
		const FVector2D& L1 = Verts[I];

		return !FGeom::ToTheLeft(p, L0, L1);
	}
	
	checkf(K >= 0 && L >= 0 && I >= 0 && J >= 0, TEXT("Assertion failed: k >= 0 && l >= 0 && i >= 0 && j >= 0"));

	const FVector2D& p = Verts[L];
	const FVector2D& C0 = Verts[K];
	const FVector2D& C1 = Verts[I];
	const FVector2D& C2 = Verts[J];

	checkf(FGeom::ToTheLeft(C2, C0, C1), TEXT("Assertion failed: Geom::ToTheLeft(c2, c0, c1)"));
	checkf(FGeom::ToTheLeft(C2, C1, p), TEXT("Assertion failed: Geom::ToTheLeft(c2, c1, p)"));

	return !FGeom::InsideCircumcircle(p, C0, C1, C2);
}

void FDelaunayCalculator::LegalizeEdge(const int Ti0, int Ti1, const int PiP, const int Li0, const int Li1)
{
	Ti1 = LeafWithEdge(Ti1, Li0, Li1);

	const FTriangleNode T0 = Triangles[Ti0];
	const FTriangleNode T1 = Triangles[Ti1];
	const int Qi = T1.OtherPoint(Li0, Li1);

	check(T0.HasEdge(Li0, Li1));
	check(T1.HasEdge(Li0, Li1));
	check(T0.IsLeaf());
	check(T1.IsLeaf());
	check(T0.P0 == PiP || T0.P1 == PiP || T0.P2 == PiP);
	check(T1.P0 == Qi || T1.P1 == Qi || T1.P2 == Qi);

	if (!LegalEdge(PiP, Qi, Li0, Li1)) 
	{
		const int Ti2 = Triangles.Num();
		const int Ti3 = Ti2 + 1;

		FTriangleNode T2 = FTriangleNode(PiP, Li0, Qi);
		FTriangleNode T3 = FTriangleNode(PiP, Qi, Li1);

		T2.A0 = T1.Opposite(Li1);
		T2.A1 = Ti3;
		T2.A2 = T0.Opposite(Li1);

		T3.A0 = T1.Opposite(Li0);
		T3.A1 = T0.Opposite(Li0);
		T3.A2 = Ti2;

		Triangles.Add(T2);
		Triangles.Add(T3);

		FTriangleNode nt0 = Triangles[Ti0];
		FTriangleNode nt1 = Triangles[Ti1];

		nt0.C0 = Ti2;
		nt0.C1 = Ti3;

		nt1.C0 = Ti2;
		nt1.C1 = Ti3;

		Triangles[Ti0] = nt0;
		Triangles[Ti1] = nt1;

		if (T2.A0 != -1) LegalizeEdge(Ti2, T2.A0, PiP, Li0, Qi);
		if (T3.A0 != -1) LegalizeEdge(Ti3, T3.A0, PiP, Qi, Li1);
	}
}

int FDelaunayCalculator::FindTriangleNode(const int PiP)
{
	int Curr = 0;

	while (!Triangles[Curr].IsLeaf())
	{
		if (const FTriangleNode& t = Triangles[Curr]; t.C0 >= 0 && PointInTriangle(PiP, t.C0))
		{
			Curr = t.C0;
		}
		else if (t.C1 >= 0 && PointInTriangle(PiP, t.C1))
		{
			Curr = t.C1;
		}
		else
		{
			Curr = t.C2;
		}
	}

	return Curr;
}

bool FDelaunayCalculator::PointInTriangle(const int PiP, const int Ti)
{
	const FTriangleNode t = Triangles[Ti];
	return ToTheLeft(PiP, t.P0, t.P1)
		&& ToTheLeft(PiP, t.P1, t.P2)
		&& ToTheLeft(PiP, t.P2, t.P0);
}

bool FDelaunayCalculator::ToTheLeft(const int PiP, const int Li0, const int Li1)
{
	if (Li0 == -2)
	{
		return Higher(Li1, PiP);
	}
	if (Li0 == -1)
	{
		return Higher(PiP, Li1);
	}
	if (Li1 == -2)
	{
		return Higher(PiP, Li0);
	}
	if (Li1 == -1)
	{
		return Higher(Li0, PiP);
	}
	
	check(Li0 >= 0);
	check(Li1 >= 0);

	return FGeom::ToTheLeft(Verts[PiP], Verts[Li0], Verts[Li1]);
}
