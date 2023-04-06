#pragma once

#include "CoreMinimal.h"
#include "FFVoronoiDiagram.h"

class FVoronoiClipper
{
public:
	TArray<FVector2D> PointsIn;
	TArray<FVector2D> PointsOut;

	void ClipSite(FFVoronoiDiagram& Diag, const TArray<FVector2D>& Polygon, const int Site, TArray<FVector2D>& Clipped);
};
