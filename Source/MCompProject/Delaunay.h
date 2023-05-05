#pragma once

#include "CoreMinimal.h"

struct FDPoint
{
	float X;

	float Y;

	int32 Id;

	// Initialize
	FDPoint(const float InX, const float InY, const int32 InId)
		: X(InX)
		, Y(InY)
		, Id(InId)
	{}

	// Functions

	float GetDistSqr(const FVector2D& InPoint) const
	{
		return FVector2D::DistSquared(InPoint, FVector2D(X, Y));
	}

	float GetDist(const FVector2D& InPoint) const
	{
		return FMath::Sqrt(GetDistSqr(InPoint));
	}
	
	bool IsNearlyEqual(const FDPoint& Other) const
	{
		return FMath::IsNearlyEqual(X, Other.X) && FMath::IsNearlyEqual(Y, Other.Y);
	}

	// Operators
	bool operator==(const FDPoint& InOther) const
	{
		return Id == InOther.Id;
	}

};


struct FDEdge
{

	FDPoint P1;

	FDPoint P2;

	// Initialize
	FDEdge(const FDPoint& InP1, const FDPoint& InP2)
		: P1(InP1)
		, P2(InP2)
	{}

	// Functions

	float GetLength() const
	{
		return FVector2D::Distance(FVector2D(P1.X, P1.Y), FVector2D(P2.X, P2.Y));
	}
	
	bool IsSimilar(const FDEdge& InEdge) const
	{
		// Returns true if P1 and P2 match in any direction
		return (P1.IsNearlyEqual(InEdge.P1) && P2.IsNearlyEqual(InEdge.P2)) || (P2.IsNearlyEqual(InEdge.P1) && P1.IsNearlyEqual(InEdge.P2));
	}

	bool IsNearlyEqual(const FDEdge& InEdge) const
	{
		return P1.IsNearlyEqual(InEdge.P1) && P2.IsNearlyEqual(InEdge.P2);
	}

	// Operators
	bool operator==(const FDEdge& InOther) const
	{
		return P1 == InOther.P1 && P2 == InOther.P2;
	}
};


struct FDTriangle
{

	FDPoint P1;

	FDPoint P2;

	FDPoint P3;

	FDEdge E1;

	FDEdge E2;

	FDEdge E3;

	// Initialize
	FDTriangle(const FDPoint& InP1, const FDPoint& InP2, const FDPoint& InP3)
		: P1(InP1)
		, P2(InP2)
		, P3(InP3)
		, E1(FDEdge(InP1, InP2))
		, E2(FDEdge(InP2, InP3))
		, E3(FDEdge(InP3, InP1))
	{}

	// Functions

	float QuatCross(float InA, float InB, float InC) const {
		return FMath::Sqrt(
			(InA + InB + InC)
			* (InA + InB - InC)
			* (InA - InB + InC)
			* (-InA + InB + InC)
		);
	}
	
	FVector GetSidesLength() const {
		return FVector(E1.GetLength(), E2.GetLength(), E3.GetLength());
	}

	FVector2D GetCenter() const {
		return FVector2D(
			(P1.X + P2.X + P3.X) / 3.f
			, (P1.Y + P2.Y + P3.Y) / 3.f
		);
	}

	float GetCircumRadius() const {
		FVector SidesLength = GetSidesLength();
		float Cross = QuatCross(SidesLength.X, SidesLength.Y, SidesLength.Z);
		// Safe division
		return (Cross != 0.f ? (SidesLength.X * SidesLength.Y * SidesLength.Z) / Cross : 0.f);
	}

	FVector2D GetCircumCenter() const
	{
		float D = (P1.X * (P2.Y - P3.Y) + P2.X * (P3.Y - P1.Y) + P3.X * (P1.Y - P2.Y)) * 2;
		float X = (P1.X * P1.X + P1.Y * P1.Y) * (P2.Y - P3.Y) + (P2.X * P2.X + P2.Y * P2.Y) * (P3.Y - P1.Y) + (P3.X * P3.X + P3.Y * P3.Y) * (P1.Y - P2.Y);
		float Y = (P1.X * P1.X + P1.Y * P1.Y) * (P3.X - P2.X) + (P2.X * P2.X + P2.Y * P2.Y) * (P1.X - P3.X) + (P3.X * P3.X + P3.Y * P3.Y) * (P2.X - P1.X);

		return D != 0.f ? FVector2D(X / D, Y / D) : FVector2D(0.f, 0.f);
	}

	float GetArea() const
	{
		FVector SidesLength = GetSidesLength();
		return QuatCross(SidesLength.X, SidesLength.Y, SidesLength.Z) / 4.f;
	}

	bool IsInCircumCircle(const FDPoint& InPoint) const
	{
		FVector2D Center = GetCircumCenter();
		return FMath::Square(Center.X - InPoint.X) + FMath::Square(Center.Y - InPoint.Y) <= FMath::Square(GetCircumRadius());
	}

	bool IsNearlyEqual(const FDTriangle& InTriangle) const
	{
		return (P1.IsNearlyEqual(InTriangle.P1)
			&& P2.IsNearlyEqual(InTriangle.P2)
			&& P3.IsNearlyEqual(InTriangle.P3)
			);
	}

	// Operators
	bool operator==(const FDTriangle& Other) const
	{
		return (
			P1 == Other.P1
			&& P2 == Other.P2
			&& P3 == Other.P3
			);
	}
};

class FDelaunay
{
public:
	TArray<FDTriangle> Triangulate(TArray<FDPoint>& InPoints, int32 InDelaunayConvexMultiplier) const;
};