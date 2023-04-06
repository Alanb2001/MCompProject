#pragma once

#include "CoreMinimal.h"

class FGeom
{
public:
	static bool AreCoincident(const FVector2D& A, const FVector2D& B);

	static bool ToTheLeft(const FVector2D& P, const FVector2D& L0, const FVector2D& L1);

	static bool ToTheRight(const FVector2D& P, const FVector2D& L0, const FVector2D& L1);

	static bool PointInTriangle(const FVector2D& P, const FVector2D& C0, const FVector2D& C1, const FVector2D& C2);

	static bool InsideCircumcircle(const FVector2D& P, const FVector2D& C0, const FVector2D& C1, const FVector2D& C2);

	static FVector2D RotateRightAngle(const FVector2D& V);

	static bool LineLineIntersection(const FVector2D P0, const FVector2D V0, const FVector2D P1, const FVector2D V1, float& M0, float& M1);

	static FVector2D LineLineIntersection(const FVector2D P0, const FVector2D V0, const FVector2D P1, const FVector2D V1);

	static FVector2D CircumcircleCenter(const FVector2D C0, const FVector2D C1, const FVector2D C2);

	static FVector2D TriangleCentroid(const FVector2D& C0, const FVector2D& C1, const FVector2D& C2);

	static float Area(const TArray<FVector2D>& Polygon);
};
