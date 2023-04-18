#pragma once

#include "CoreMinimal.h"

class FDelaunayTriangulation
{
public:
	TArray<FVector2D> Vertices;

	TArray<int> Triangles;

	FDelaunayTriangulation();

	void Clear();
};
