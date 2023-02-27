#pragma once

#include "CoreMinimal.h"
#include "Geom.h"

class DelaunayTriangulation
{
public:
	TArray<FVector2D> Vertices;

	TArray<int32> Triangles;

	DelaunayTriangulation();

	void Clear();

	bool Verify() const;
};
