#pragma once

#include "CoreMinimal.h"

class Geom
{
public:
	static bool AreCoincident(const FVector2D& a, const FVector2D& b);

	static bool ToTheLeft(const FVector2D& p, const FVector2D& l0, const FVector2D& l1);

	static bool ToTheRight(const FVector2D& p, const FVector2D& l0, const FVector2D& l1);

	static bool PointInTriangle(const FVector2D& p, const FVector2D& c0, const FVector2D& c1, const FVector2D& c2);

	static bool InsideCircumcircle(const FVector2D& p, const FVector2D& c0, const FVector2D& c1, const FVector2D& c2);

	static FVector2D RotateRightAngle(const FVector2D& v);

	static bool LineLineIntersection(FVector2D p0, FVector2D v0, FVector2D p1, FVector2D v1, float& m0, float& m1);

	static FVector2D LineLineIntersection(FVector2D p0, FVector2D v0, FVector2D p1, FVector2D v1);

	static FVector2D CircumcircleCenter(FVector2D c0, FVector2D c1, FVector2D c2);

	static FVector2D TriangleCentroid(const FVector2D& c0, const FVector2D& c1, const FVector2D& c2);

	static float Area(const TArray<FVector2D>& polygon);
};
