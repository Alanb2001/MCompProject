#pragma once

#include "CoreMinimal.h"
#include "DelaunayTriangulation.h"
#include <string>

struct TriangleNode
{
	int32 P0;
	int32 P1;
	int32 P2;

	int32 C0;
	int32 C1;
	int32 C2;

	int32 A0;
	int32 A1;
	int32 A2;

	bool IsLeaf() const
	{
		return C0 < 0 && C1 < 0 && C2 < 0;
	}

	bool IsInner() const
	{
		return P0 >= 0 && P1 >= 0 && P2 >= 0;
	}

	TriangleNode(int32 P0 = -1, int32 P1 = -1, int32 P2 = -1)
		: P0(P0)
		, P1(P1)
		, P2(P2)
		, C0(-1)
		, C1(-1)
		, C2(-1)
		, A0(-1)
		, A1(-1)
		, A2(-1)
	{
	}

	bool HasEdge(int32 e0, int32 e1) const
	{
		if (e0 == P0)
		{
			return e1 == P1 || e1 == P2;
		}
		else if (e0 == P1)
		{
			return e1 == P0 || e1 == P2;
		}
		else if (e0 == P2)
		{
			return e1 == P0 || e1 == P1;
		}

		return false;
	}

	int32 OtherPoint(int32 p0, int32 p1)
	{
		if (p0 == P0)
		{
			if (p1 == P1)
				return P2;
			if (p1 == P2)
				return P1;
			throw std::invalid_argument("p0 and p1 not on triangle");
		}
		if (p0 == P1)
		{
			if (p1 == P0)
				return P2;
			if (p1 == P2)
				return P0;
			throw std::invalid_argument("p0 and p1 not on triangle");
		}
		if (p0 == P2)
		{
			if (p1 == P0)
				return P1;
			if (p1 == P1)
				return P0;
			throw std::invalid_argument("p0 and p1 not on triangle");
		}

		throw std::invalid_argument("p0 and p1 not on triangle");
	}

	int32 Opposite(int32 p)
	{
		if (p == P0)
			return A0;
		if (p == P1)
			return A1;
		if (p == P2)
			return A2;
		throw std::invalid_argument("p not in triangle");
	}

	FString ToString() const
	{
		if (IsLeaf())
		{
			return FString::Printf(TEXT("TriangleNode(%d, %d, %d)"), P0, P1, P2);
		}
		else
		{
			return FString::Printf(TEXT("TriangleNode(%d, %d, %d, %d, %d, %d)"), P0, P1, P2, C0, C1, C2);
		}
	}
};

class DelaunayCalculator
{
public:
	int32 highest = -1;
	TArray<FVector2D> verts;

	TArray<int32> indices;
	TArray<TriangleNode> triangles;

	DelaunayCalculator();

	DelaunayTriangulation CalculateTriangulation(const TArray<FVector2D>& verts1);

	void CalculateTriangulation(const TArray<FVector2D>& verts1, DelaunayTriangulation& result);

	bool Higher(int32 pi0, int32 pi1);

	void RunBowyerWatson();

	void GenerateResult(DelaunayTriangulation& result);

	void ShuffleIndices();

	int LeafWithEdge(int32 ti, int32 e0, int32 e1);

	bool LegalEdge(int32 k, int32 l, int32 i, int32 j);

	void LegalizeEdge(int32 ti0, int32 ti1, int32 pi, int32 li0, int32 li1);

	int FindTriangleNode(int pi);

	bool PointInTriangle(int pi, int ti);

	bool ToTheLeft(int pi, int li0, int li1);
};
