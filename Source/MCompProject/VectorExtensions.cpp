#include "VectorExtensions.h"

bool FVectorExtensions::IsReal(const float F) 
{
    return !std::isinf(F) && !std::isnan(F);
}

bool FVectorExtensions::IsReal(const FVector2D& V2) 
{
    return IsReal(V2.X) && IsReal(V2.Y);
}

bool FVectorExtensions::IsReal(const FVector& V3) 
{
    return IsReal(V3.X) && IsReal(V3.Y) && IsReal(V3.Z);
}
