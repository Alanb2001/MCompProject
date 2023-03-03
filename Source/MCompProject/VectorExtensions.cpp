#include "VectorExtensions.h"

bool VectorExtensions::IsReal(float f) 
{
    return !std::isinf(f) && !std::isnan(f);
}

bool VectorExtensions::IsReal(const FVector2D& v2) 
{
    return IsReal(v2.X) && IsReal(v2.Y);
}

bool VectorExtensions::IsReal(const FVector& v3) 
{
    return IsReal(v3.X) && IsReal(v3.Y) && IsReal(v3.Z);
}
