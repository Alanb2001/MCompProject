#pragma once

#include "CoreMinimal.h"
#include "VoronoiDiagram.h"

class VoronoiClipper
{
public:
	TArray<FVector2D> pointsIn;
	TArray<FVector2D> pointsOut;

	void ClipSite(VoronoiDiagram diag, TArray<FVector2D> polygon, int site, TArray<FVector2D>& clipped);
};
