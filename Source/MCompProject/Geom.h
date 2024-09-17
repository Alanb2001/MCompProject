#pragma once

#include "CoreMinimal.h"

class FGeom
{
public:
	static bool AreCoincident(FVector2D A, FVector2D B);

	static bool ToTheLeft(FVector2D P, FVector2D L0, FVector2D L1);

	static bool ToTheRight(FVector2D P, FVector2D L0, FVector2D L1);

	static bool PointInTriangle(FVector2D P, FVector2D C0, FVector2D C1, FVector2D C2);

	static bool InsideCircumcircle(FVector2D P, FVector2D C0, FVector2D C1, FVector2D C2);

	static FVector2D RotateRightAngle(FVector2D V);

	static bool LineLineIntersection(FVector2D P0, FVector2D V0, FVector2D P1, FVector2D V1, float& M0, float& M1);

	static FVector2D LineLineIntersection(FVector2D P0, FVector2D V0, FVector2D P1, FVector2D V1);

	static FVector2D CircumcircleCenter(FVector2D C0, FVector2D C1, FVector2D C2);

	static FVector2D TriangleCentroid(FVector2D C0, FVector2D C1, FVector2D C2);

	static float Area(TArray<FVector2D> Polygon);
};
