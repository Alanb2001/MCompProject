#pragma once

#include "CoreMinimal.h"

class VectorExtensions
{
public:
	static bool IsReal(float f);

	static bool IsReal(FVector2D v2);

	static bool IsReal(const FVector& v3);
};
