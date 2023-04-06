#pragma once

#include "CoreMinimal.h"
#include "DelaunayTriangulation.h"

struct FTriangleNode
{
	int P0;
	int P1;
	int P2;

	int C0;
	int C1;
	int C2;

	int A0;
	int A1;
	int A2;

	bool IsLeaf() const
	{
		return C0 < 0 && C1 < 0 && C2 < 0;
	}

	bool IsInner() const
	{
		return P0 >= 0 && P1 >= 0 && P2 >= 0;
	}

	FTriangleNode(const int P0, const int P1, const int P2)
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

	bool HasEdge(const int E0, const int E1) const
	{
		if (E0 == P0)
		{
			return E1 == P1 || E1 == P2;
		}
		if (E0 == P1)
		{
			return E1 == P0 || E1 == P2;
		}
		if (E0 == P2)
		{
			return E1 == P0 || E1 == P1;
		}

		return false;
	}

	int OtherPoint(const int P0P, const int P1P) const
	{
		if (P0P == P0)
		{
			if (P1P == P1)
			{
				return P2;
			}
			if (P1P == P2)
			{
				return P1;
			}
			throw std::invalid_argument("p0 and p1 not on triangle");
		}
		if (P0P == P1)
		{
			if (P1P == P0)
			{
				return P2;
			}
			if (P1P == P2)
			{
				return P0;
			}
			throw std::invalid_argument("p0 and p1 not on triangle");
		}
		if (P0P == P2)
		{
			if (P1P == P0)
			{
				return P1;
			}
			if (P1P == P1)
			{
				return P0;
			}

			throw std::invalid_argument("p0 and p1 not on triangle");
		}

		throw std::invalid_argument("p0 and p1 not on triangle");
	}

	int Opposite(const int P) const
	{
		if (P == P0)
		{
			return A0;
		}
		if (P == P1)
		{
			return A1;
		}
		if (P == P2)
		{
			return A2;
		}
		throw std::invalid_argument("p not in triangle");
	}

	FString ToString() const
	{
		if (IsLeaf())
		{
			return FString::Printf(TEXT("TriangleNode(%d, %d, %d)"), P0, P1, P2);
		}
		return FString::Printf(TEXT("TriangleNode(%d, %d, %d, %d, %d, %d)"), P0, P1, P2, C0, C1, C2);
	}
};

class FDelaunayCalculator
{
public:
	int Highest = -1;
	TArray<FVector2D> Verts;

	TArray<int> Indices;
	TArray<FTriangleNode> Triangles;

	FDelaunayCalculator();

	FDelaunayTriangulation* CalculateTriangulation(const TArray<FVector2D>& Verts1);

	void CalculateTriangulation(const TArray<FVector2D>& Verts1, FDelaunayTriangulation* Result);

	bool Higher(const int PI0, const int PI1);

	void RunBowyerWatson();

	void GenerateResult(FDelaunayTriangulation* Result);

	void ShuffleIndices();

	int LeafWithEdge(int Ti, const int E0, const int E1);

	bool LegalEdge(const int K, const int L, const int I, const int J);

	void LegalizeEdge(const int Ti0, int Ti1, const int PiP, const int Li0, const int Li1);

	int FindTriangleNode(const int PiP);

	bool PointInTriangle(const int PiP, const int Ti);

	bool ToTheLeft(const int PiP, const int Li0, const int Li1);
};
