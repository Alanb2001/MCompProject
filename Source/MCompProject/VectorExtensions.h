#pragma once

#include "CoreMinimal.h"

class FVectorExtensions
{
public:
	static bool IsReal(const float F);

	static bool IsReal(const FVector2D& V2);

	static bool IsReal(const FVector& V3);
};
