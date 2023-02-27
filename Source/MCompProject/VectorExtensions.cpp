#include "VectorExtensions.h"

bool VectorExtensions::IsReal(float f)
{
	return std::isfinite(f) && !std::isnan(f);
}

bool VectorExtensions::IsReal(FVector2D v2)
{
	return FMath::IsFinite(v2.X) && FMath::IsFinite(v2.Y);
}

bool VectorExtensions::IsReal(const FVector& v3)
{
	return FMath::IsFinite(v3.X) && FMath::IsFinite(v3.Y) && FMath::IsFinite(v3.Z);
}
