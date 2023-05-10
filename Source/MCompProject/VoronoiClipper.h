#pragma once

#include "CoreMinimal.h"
#include "FFVoronoiDiagram.h"

class FVoronoiClipper
{
public:
	TArray<FVector2D> PointsIn;
	TArray<FVector2D> PointsOut;

	void ClipSite(FFVoronoiDiagram& Diag, TArray<FVector2D>& Polygon, int& Site, TArray<FVector2D>& Clipped);
};
