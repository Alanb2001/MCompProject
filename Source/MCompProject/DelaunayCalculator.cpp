#include "DelaunayCalculator.h"
#include "Geom.h"

FDelaunayCalculator::FDelaunayCalculator()
{
	Triangles = TArray<FTriangleNode>();
	indices = TArray<int>();
}

void FDelaunayCalculator::CalculateTriangulation(TArray<FVector2D> Verts1, FDelaunayTriangulation* Result)
{
	if (Verts1.Num() == NULL) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Points"));
	}
	
	if (Verts1.Num() < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("You need at least 3 points for a triangulation"));
	}
	
	Triangles.Empty();
	this->verts = Verts1;
	
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

	this->verts.Empty();
}

bool FDelaunayCalculator::Higher(int PI0, int PI1)
{
	if (PI0 == -2)
	{
		return false;
	}
	else if (PI0 == -1)
	{
		return true;
	}
	else if (PI1 == -2)
	{
		return true;
	}
	else if (PI1 == -1)
	{
		return false;
	}
	else
	{
		FVector2D P0 = verts[PI0];
		FVector2D P1 = verts[PI1];

		if (P0.Y < P1.Y)
		{
			return true;
		}
		else if (P0.Y > P1.Y)
		{
			return false;
		}
		else
		{
			return P0.X < P1.X;
		}
	}
}

void FDelaunayCalculator::RunBowyerWatson()
{
	for (int i = 0; i < verts.Num(); i++)
	{
		int PiP = i;
		
		if (PiP == Highest)
		{
			continue;
		}

		int Ti = FindTriangleNode(PiP);
		
		FTriangleNode t = Triangles[Ti];

		int P0 = t.P0;
		int P1 = t.P1;
		int P2 = t.P2;

		int Nti0 = Triangles.Num();
		int Nti1 = Nti0 + 1;
		int Nti2 = Nti0 + 2;

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

		if (Nt0.A0 != -1)
		{
			LegaliseEdge(Nti0, Nt0.A0, PiP, P0, P1);
		}
		if (Nt1.A0 != -1)
		{
			LegaliseEdge(Nti1, Nt1.A0, PiP, P1, P2);
		}
		if (Nt2.A0 != -1)
		{
			LegaliseEdge(Nti2, Nt2.A0, PiP, P2, P0);
		}
	}
}

void FDelaunayCalculator::GenerateResult(FDelaunayTriangulation* Result)
{
	if (Result == nullptr)
	{
		Result = new FDelaunayTriangulation();
	}

	Result->Clear();

	for (int i = 0; i < verts.Num(); i++)
	{
		Result->Vertices.Add(verts[i]);
	}

	for (int i = 1; i < Triangles.Num(); i++)
	{
		FTriangleNode& t = Triangles[i];

		if (t.IsLeaf() && t.IsInner())
		{
			Result->Triangles.Add(t.P0);
			Result->Triangles.Add(t.P1);
			Result->Triangles.Add(t.P2);
		}
	}
}

void FDelaunayCalculator::ShuffleIndices()
{
	indices.Empty();
	indices.Reserve(verts.Num());

	for (int i = 0; i < verts.Num(); i++)
	{
		indices.Add(i);
	}

	assert(indices.Num() == verts.Num());

	for (int i = 0; i < verts.Num() - 1; i++)
	{
		int j = FMath::RandRange(i, verts.Num() - 1);
		
		indices.Swap(i, j);
	}
}

int FDelaunayCalculator::LeafWithEdge(int Ti, int E0, int E1)
{
	assert(Triangles[Ti].HasEdge(E0, E1));

	while (!Triangles[Ti].IsLeaf())
	{
		FTriangleNode& t = Triangles[Ti];

		if (t.C0 != -1 && Triangles[t.C0].HasEdge(E0, E1))
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
			assert(false);
			throw std::runtime_error("This should never happen");
		}
	}

	return Ti;
}

bool FDelaunayCalculator::LegalEdge(int K, int L, int I, int J)
{
	assert(K != Highest && K >= 0);

	bool bLMagic = L < 0;
	bool bIMagic = I < 0;
	bool bJMagic = J < 0;

	assert(!(bIMagic && bJMagic));

	if (bLMagic)
	{
		return true;
	}
	else if (bIMagic)
	{
		assert(!bJMagic);

		FVector2D p = verts[L];
		FVector2D L0 = verts[K];
		FVector2D L1 = verts[J];

		return FGeom::ToTheLeft(p, L0, L1);
	}
	else if (bJMagic)
	{
		assert(!bIMagic);

		FVector2D p = verts[L];
		FVector2D L0 = verts[K];
		FVector2D L1 = verts[I];

		return !FGeom::ToTheLeft(p, L0, L1);
	}
	else
	{
		assert(K >= 0 && L >= 0 && I >= 0 && J >= 0);

		FVector2D p = verts[L];
		FVector2D C0 = verts[K];
		FVector2D C1 = verts[I];
		FVector2D C2 = verts[J];

		assert(FGeom::ToTheLeft(C2, C0, C1));
		assert(FGeom::ToTheLeft(C2, C1, p));

		return !FGeom::InsideCircumcircle(p, C0, C1, C2);
	}
}

void FDelaunayCalculator::LegaliseEdge(int Ti0, int Ti1, int PiP, int Li0, int Li1)
{
	Ti1 = LeafWithEdge(Ti1, Li0, Li1);

	FTriangleNode T0 = Triangles[Ti0];
	FTriangleNode T1 = Triangles[Ti1];
	int Qi = T1.OtherPoint(Li0, Li1);

	assert(T0.HasEdge(Li0, Li1));
	assert(T1.HasEdge(Li0, Li1));
	assert(T0.IsLeaf());
	assert(T1.IsLeaf());
	assert(T0.P0 == PiP || T0.P1 == PiP || T0.P2 == PiP);
	assert(T1.P0 == Qi || T1.P1 == Qi || T1.P2 == Qi);

	if (!LegalEdge(PiP, Qi, Li0, Li1)) 
	{
		int Ti2 = Triangles.Num();
		int Ti3 = Ti2 + 1;

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

		if (T2.A0 != -1)
		{
			LegaliseEdge(Ti2, T2.A0, PiP, Li0, Qi);
		}

		if (T3.A0 != -1)
		{
			LegaliseEdge(Ti3, T3.A0, PiP, Qi, Li1);
		}
	}
}

int FDelaunayCalculator::FindTriangleNode(int PiP)
{
	int Curr = 0;

	while (!Triangles[Curr].IsLeaf())
	{
		FTriangleNode& t = Triangles[Curr];

		if (t.C0 >= 0 && PointInTriangle(PiP, t.C0))
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

bool FDelaunayCalculator::PointInTriangle(int PiP, int Ti)
{
	FTriangleNode t = Triangles[Ti];
	return ToTheLeft(PiP, t.P0, t.P1)
		&& ToTheLeft(PiP, t.P1, t.P2)
		&& ToTheLeft(PiP, t.P2, t.P0);
}

bool FDelaunayCalculator::ToTheLeft(int PiP, int Li0, int Li1)
{
	if (Li0 == -2)
	{
		return Higher(Li1, PiP);
	}
	else if (Li0 == -1)
	{
		return Higher(PiP, Li1);
	}
	else if (Li1 == -2)
	{
		return Higher(PiP, Li0);
	}
	else if (Li1 == -1)
	{
		return Higher(Li0, PiP);
	}
	else
	{
		assert(Li0 >= 0);
		assert(Li1 >= 0);

		return FGeom::ToTheLeft(verts[PiP], verts[Li0], verts[Li1]);
	}
}
