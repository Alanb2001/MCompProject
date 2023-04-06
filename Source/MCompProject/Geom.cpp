#include "Geom.h"

bool FGeom::AreCoincident(const FVector2D& A, const FVector2D& B)
{
    return (A - B).Size() < 0.000001f;
}

bool FGeom::ToTheLeft(const FVector2D& P, const FVector2D& L0, const FVector2D& L1)
{
    return (L1.X - L0.X) * (P.Y - L0.Y) - (L1.Y - L0.Y) * (P.X - L0.X) >= 0;
}

bool FGeom::ToTheRight(const FVector2D& P, const FVector2D& L0, const FVector2D& L1)
{
    return !ToTheLeft(P, L0, L1);
}

bool FGeom::PointInTriangle(const FVector2D& P, const FVector2D& C0, const FVector2D& C1, const FVector2D& C2)
{
    return ToTheLeft(P, C0, C1)
        && ToTheLeft(P, C1, C2)
        && ToTheLeft(P, C2, C0);
}

bool FGeom::InsideCircumcircle(const FVector2D& P, const FVector2D& C0, const FVector2D& C1, const FVector2D& C2)
{
    const float Ax = C0.X - P.X;
    const float AY = C0.Y - P.Y;
    const float Bx = C1.X - P.X;
    const float By = C1.Y - P.Y;
    const float Cx = C2.X - P.X;
    const float Cy = C2.Y - P.Y;

    const float Det = (Ax * Ax + AY * AY) * (Bx * Cy - Cx * By)
        - (Bx * Bx + By * By) * (Ax * Cy - Cx * AY)
        + (Cx * Cx + Cy * Cy) * (Ax * By - Bx * AY);

    return Det > 0.000001f;
}

FVector2D FGeom::RotateRightAngle(const FVector2D& V)
{
    const float x = V.X;
    const float y = V.Y;
    return FVector2D(-y, x);
}

bool FGeom::LineLineIntersection(const FVector2D P0, const FVector2D V0, const FVector2D P1, const FVector2D V1, float& M0, float& M1)
{
    if (const float Det = V0.X * V1.Y - V0.Y * V1.X; FMath::Abs(Det) < 0.001f)
    {
        M0 = NAN;
        M1 = NAN;
        return false;
    }
    else
    {
        M0 = ((P0.Y - P1.Y) * V1.X - (P0.X - P1.X) * V1.Y) / Det;

        if (FMath::Abs(V1.X) >= 0.001f)
        {
            M1 = (P0.X + M0 * V0.X - P1.X) / V1.X;
        }
        else
        {
            M1 = (P0.Y + M0 * V0.Y - P1.Y) / V1.Y;
        }

        return true;
    }
}

FVector2D FGeom::LineLineIntersection(const FVector2D P0, const FVector2D V0, const FVector2D P1, const FVector2D V1)
{
    float M1;

    if (float M0; LineLineIntersection(P0, V0, P1, V1, M0, M1)) 
    {
        return P0 + M0 * V0;
    }
    
    return FVector2D(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
}

FVector2D FGeom::CircumcircleCenter(const FVector2D C0, const FVector2D C1, const FVector2D C2)
{
    const FVector2D MP0 = 0.5f * (C0 + C1);
    const FVector2D MP1 = 0.5f * (C1 + C2);

    const FVector2D V0 = RotateRightAngle(C0 - C1);
    const FVector2D V1 = RotateRightAngle(C1 - C2);

    float M0, M1;

    LineLineIntersection(MP0, V0, MP1, V1, M0, M1);

    return MP0 + M0 * V0;
}

FVector2D FGeom::TriangleCentroid(const FVector2D& C0, const FVector2D& C1, const FVector2D& C2)
{
    const FVector2D Val = 1.0f / 3.0f * (C0 + C1 + C2);
    return Val;
}

float FGeom::Area(const TArray<FVector2D>& Polygon)
{
    float Area = 0.0f;
    const int Count = Polygon.Num();

    for (int i = 0; i < Count; i++)
    {
        const int j = i == Count - 1 ? 0 : i + 1;

        const FVector2D P0 = Polygon[i];
        const FVector2D P1 = Polygon[j];

        Area += P0.X * P1.Y - P1.X * P0.Y;
    }

    return 0.5f * Area;
}