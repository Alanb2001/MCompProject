#include "DelaunayCalculator.h"

DelaunayCalculator::DelaunayCalculator()
{
	triangles = TArray<TriangleNode>();
	indices = TArray<int32>();
}

DelaunayTriangulation DelaunayCalculator::CalculateTriangulation(const TArray<FVector2D>& verts1)
{
	DelaunayTriangulation result;

	CalculateTriangulation(verts, result);

	return result;
}

void DelaunayCalculator::CalculateTriangulation(const TArray<FVector2D>& verts1, DelaunayTriangulation& result)
{
	if (verts.Num() < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("You need at least 3 points for a triangulation"));
		return;
	}

	triangles.Empty();
	this->verts = verts;

	highest = 0;

	for (int i = 0; i < verts.Num(); i++)
	{
		if (Higher(highest, i))
		{
			highest = i;
		}
	}

	triangles.Add(TriangleNode(-2, -1, highest));

	RunBowyerWatson();
	GenerateResult(result);

	this->verts.Empty();
}

bool DelaunayCalculator::Higher(int32 pi0, int32 pi1)
{
	if (pi0 == -2)
	{
		return false;
	}
	else if (pi0 == -1)
	{
		return true;
	}
	else if (pi1 == -2)
	{
		return true;
	}
	else if (pi1 == -1)
	{
		return false;
	}
	else
	{
		FVector2D p0 = verts[pi0];
		FVector2D p1 = verts[pi1];

		if (p0.Y < p1.Y)
		{
			return true;
		}
		else if (p0.Y > p1.Y)
		{
			return false;
		}
		else
		{
			return p0.X < p1.X;
		}
	}
}

void DelaunayCalculator::RunBowyerWatson()
{
	for (int i = 0; i < verts.Num(); i++)
	{
		int pi = i;
		if (pi == highest) continue;

		int ti = FindTriangleNode(pi);
		TriangleNode t = triangles[ti];

		int32 p0 = t.P0;
		int32 p1 = t.P1;
		int32 p2 = t.P2;

		int nti0 = triangles.Num();
		int nti1 = nti0 + 1;
		int nti2 = nti0 + 2;

		TriangleNode nt0(pi, p0, p1);
		TriangleNode nt1(pi, p1, p2);
		TriangleNode nt2(pi, p2, p0);

		nt0.A0 = t.A2;
		nt1.A0 = t.A0;
		nt2.A0 = t.A1;

		nt0.A1 = nti1;
		nt1.A1 = nti2;
		nt2.A1 = nti0;

		nt0.A2 = nti2;
		nt1.A2 = nti0;
		nt2.A2 = nti1;

		t.C0 = nti0;
		t.C1 = nti1;
		t.C2 = nti2;

		triangles[ti] = t;

		triangles.Add(nt0);
		triangles.Add(nt1);
		triangles.Add(nt2);

		if (nt0.A0 != -1) LegalizeEdge(nti0, nt0.A0, pi, p0, p1);
		if (nt1.A0 != -1) LegalizeEdge(nti1, nt1.A0, pi, p1, p2);
		if (nt2.A0 != -1) LegalizeEdge(nti2, nt2.A0, pi, p2, p0);
	}
}

void DelaunayCalculator::GenerateResult(DelaunayTriangulation& result)
{
	if (result.Vertices.Num() > 0)
	{
		result.Vertices.Empty();
	}
	if (result.Triangles.Num() > 0)
	{
		result.Triangles.Empty();
	}

	for (int i = 0; i < verts.Num(); i++)
	{
		result.Vertices.Add(verts[i]);
	}

	for (int i = 1; i < triangles.Num(); i++)
	{
		const TriangleNode& t = triangles[i];

		if (t.IsLeaf() && t.IsInner()) {
			result.Triangles.Add(t.P0);
			result.Triangles.Add(t.P1);
			result.Triangles.Add(t.P2);
		}
	}
}

void DelaunayCalculator::ShuffleIndices()
{
	indices.Empty();
	indices.Reserve(verts.Num());

	for (int i = 0; i < verts.Num(); i++)
	{
		indices.Add(i);
	}

	check(indices.Num() == verts.Num());

	for (int i = 0; i < verts.Num() - 1; i++)
	{
		int j = FMath::RandRange(i, verts.Num() - 1);

		int32 tmp = indices[i];
		indices[i] = indices[j];
		indices[j] = tmp;
	}
}

int DelaunayCalculator::LeafWithEdge(int32 ti, int32 e0, int32 e1)
{
	check(triangles[ti].HasEdge(e0, e1));

	while (!triangles[ti].IsLeaf())
	{
		auto t = triangles[ti];

		if (t.C0 != -1 && triangles[t.C0].HasEdge(e0, e1))
		{
			ti = t.C0;
		}
		else if (t.C1 != -1 && triangles[t.C1].HasEdge(e0, e1))
		{
			ti = t.C1;
		}
		else if (t.C2 != -1 && triangles[t.C2].HasEdge(e0, e1))
		{
			ti = t.C2;
		}
		else
		{
			check(false);
			throw std::exception("This should never happen");
		}
	}

	return ti;
}

bool DelaunayCalculator::LegalEdge(int32 k, int32 l, int32 i, int32 j)
{
	checkf(k != highest && k >= 0, TEXT("Assertion failed: k != highest && k >= 0"));

	const bool lMagic = l < 0;
	const bool iMagic = i < 0;
	const bool jMagic = j < 0;

	checkf(!(iMagic && jMagic), TEXT("Assertion failed: !(iMagic && jMagic)"));

	if (lMagic)
	{
		return true;
	}
	else if (iMagic)
	{
		checkf(!jMagic, TEXT("Assertion failed: !jMagic"));

		const FVector2D& p = verts[l];
		const FVector2D& l0 = verts[k];
		const FVector2D& l1 = verts[j];

		return Geom::ToTheLeft(p, l0, l1);
	}
	else if (jMagic)
	{
		checkf(!iMagic, TEXT("Assertion failed: !iMagic"));

		const FVector2D& p = verts[l];
		const FVector2D& l0 = verts[k];
		const FVector2D& l1 = verts[i];

		return !Geom::ToTheLeft(p, l0, l1);
	}
	else
	{
		checkf(k >= 0 && l >= 0 && i >= 0 && j >= 0, TEXT("Assertion failed: k >= 0 && l >= 0 && i >= 0 && j >= 0"));

		const FVector2D& p = verts[l];
		const FVector2D& c0 = verts[k];
		const FVector2D& c1 = verts[i];
		const FVector2D& c2 = verts[j];

		checkf(Geom::ToTheLeft(c2, c0, c1), TEXT("Assertion failed: Geom::ToTheLeft(c2, c0, c1)"));
		checkf(Geom::ToTheLeft(c2, c1, p), TEXT("Assertion failed: Geom::ToTheLeft(c2, c1, p)"));

		return !Geom::InsideCircumcircle(p, c0, c1, c2);
	}
}

void DelaunayCalculator::LegalizeEdge(int32 ti0, int32 ti1, int32 pi, int32 li0, int32 li1)
{
	ti1 = LeafWithEdge(ti1, li0, li1);

	TriangleNode t0 = triangles[ti0];
	TriangleNode t1 = triangles[ti1];
	int qi = t1.OtherPoint(li0, li1);

	check(t0.HasEdge(li0, li1));
	check(t1.HasEdge(li0, li1));
	check(t0.IsLeaf());
	check(t1.IsLeaf());
	check(t0.P0 == pi || t0.P1 == pi || t0.P2 == pi);
	check(t1.P0 == qi || t1.P1 == qi || t1.P2 == qi);

	if (!LegalEdge(pi, qi, li0, li1)) {
		int32 ti2 = triangles.Num();
		int ti3 = ti2 + 1;

		TriangleNode t2 = TriangleNode(pi, li0, qi);
		TriangleNode t3 = TriangleNode(pi, qi, li1);

		t2.A0 = t1.Opposite(li1);
		t2.A1 = ti3;
		t2.A2 = t0.Opposite(li1);

		t3.A0 = t1.Opposite(li0);
		t3.A1 = t0.Opposite(li0);
		t3.A2 = ti2;

		triangles.Add(t2);
		triangles.Add(t3);

		TriangleNode nt0 = triangles[ti0];
		TriangleNode nt1 = triangles[ti1];

		nt0.C0 = ti2;
		nt0.C1 = ti3;

		nt1.C0 = ti2;
		nt1.C1 = ti3;

		triangles[ti0] = nt0;
		triangles[ti1] = nt1;

		if (t2.A0 != -1) LegalizeEdge(ti2, t2.A0, pi, li0, qi);
		if (t3.A0 != -1) LegalizeEdge(ti3, t3.A0, pi, qi, li1);
	}
}

int DelaunayCalculator::FindTriangleNode(int pi)
{
	int curr = 0;

	while (!triangles[curr].IsLeaf())
	{
		const TriangleNode& t = triangles[curr];

		if (t.C0 >= 0 && PointInTriangle(pi, t.C0))
		{
			curr = t.C0;
		}
		else if (t.C1 >= 0 && PointInTriangle(pi, t.C1))
		{
			curr = t.C1;
		}
		else
		{
			curr = t.C2;
		}
	}

	return curr;
}

bool DelaunayCalculator::PointInTriangle(int pi, int ti)
{
	TriangleNode t = triangles[ti];
	return ToTheLeft(pi, t.P0, t.P1)
		&& ToTheLeft(pi, t.P1, t.P2)
		&& ToTheLeft(pi, t.P2, t.P0);
}

bool DelaunayCalculator::ToTheLeft(int pi, int li0, int li1)
{
	if (li0 == -2)
	{
		return Higher(li1, pi);
	}
	else if (li0 == -1)
	{
		return Higher(pi, li1);
	}
	else if (li1 == -2)
	{
		return Higher(pi, li0);
	}
	else if (li1 == -1)
	{
		return Higher(li0, pi);
	}
	else
	{
		check(li0 >= 0);
		check(li1 >= 0);

		return Geom::ToTheLeft(verts[pi], verts[li0], verts[li1]);
	}
}
