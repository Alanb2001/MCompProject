#pragma once

#include "CoreMinimal.h"
#include "DelaunayTriangulation.h"

#include <stdexcept>
#include <string>
#include <cassert>
#include <iostream>

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

	bool IsLeaf()
	{
		return C0 < 0 && C1 < 0 && C2 < 0;
	}

	bool IsInner()
	{
		return P0 >= 0 && P1 >= 0 && P2 >= 0;
	}

	FTriangleNode(int P0, int P1, int P2)
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

	bool HasEdge(int E0, int E1)
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

	int OtherPoint(int P0P, int P1P)
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

	int Opposite(int P)
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
};

class FDelaunayCalculator
{
public:
	int Highest = -1;
	TArray<FVector2D> verts;

	TArray<int> indices;
	TArray<FTriangleNode> Triangles;

	FDelaunayCalculator();
	
	void CalculateTriangulation(TArray<FVector2D> Verts1, FDelaunayTriangulation* Result);

	bool Higher(int PI0, int PI1);

	void RunBowyerWatson();

	void GenerateResult(FDelaunayTriangulation* Result);
	
	void ShuffleIndices();

	int LeafWithEdge(int Ti, int E0, int E1);

	bool LegalEdge(int K, int L, int I, int J);

	void LegaliseEdge(int Ti0, int Ti1, int PiP, int Li0, int Li1);

	int FindTriangleNode(int PiP);

	bool PointInTriangle(int PiP, int Ti);

	bool ToTheLeft(int PiP, int Li0, int Li1);
};
