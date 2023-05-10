#include "Geom.h"

bool FGeom::AreCoincident(FVector2D A, FVector2D B)
{
    return (A - B).Size() < 0.000001f;
}

bool FGeom::ToTheLeft(FVector2D P, FVector2D L0, FVector2D L1)
{
    return (L1.X - L0.X) * (P.Y - L0.Y) - (L1.Y - L0.Y) * (P.X - L0.X) >= 0;
}

bool FGeom::InsideCircumcircle(FVector2D P, FVector2D C0, FVector2D C1, FVector2D C2)
{
    float Ax = C0.X - P.X;
    float AY = C0.Y - P.Y;
    float Bx = C1.X - P.X;
    float By = C1.Y - P.Y;
    float Cx = C2.X - P.X;
    float Cy = C2.Y - P.Y;

    float Det = (Ax * Ax + AY * AY) * (Bx * Cy - Cx * By)
        - (Bx * Bx + By * By) * (Ax * Cy - Cx * AY)
        + (Cx * Cx + Cy * Cy) * (Ax * By - Bx * AY);

    return Det > 0.000001f;
}

FVector2D FGeom::RotateRightAngle(FVector2D V)
{
    float x = V.X;
    V.X = -V.Y;
    V.Y = x;
    return V;
}

bool FGeom::LineLineIntersection(FVector2D P0, FVector2D V0, FVector2D P1, FVector2D V1, float& M0, float& M1)
{
    if (float Det = V0.X * V1.Y - V0.Y * V1.X; FMath::Abs(Det) < 0.001f)
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

FVector2D FGeom::LineLineIntersection(FVector2D P0, FVector2D V0, FVector2D P1, FVector2D V1)
{
    float M1;

    if (float M0; LineLineIntersection(P0, V0, P1, V1, M0, M1)) 
    {
        return P0 + M0 * V0;
    }
    
    return FVector2D(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
}

FVector2D FGeom::CircumcircleCenter(FVector2D C0, FVector2D C1, FVector2D C2)
{
    FVector2D MP0 = 0.5f * (C0 + C1);
    FVector2D MP1 = 0.5f * (C1 + C2);

    FVector2D V0 = RotateRightAngle(C0 - C1);
    FVector2D V1 = RotateRightAngle(C1 - C2);

    float M0, M1;

    LineLineIntersection(MP0, V0, MP1, V1, M0, M1);

    return MP0 + M0 * V0;
}

FVector2D FGeom::TriangleCentroid(FVector2D C0, FVector2D C1, FVector2D C2)
{
    FVector2D Val = 1.0f / 3.0f * (C0 + C1 + C2);
    return Val;
}

float FGeom::Area(TArray<FVector2D> Polygon)
{
    float Area = 0.0f;
    int Count = Polygon.Num();

    for (int i = 0; i < Count; i++)
    {
        int j = i == Count - 1 ? 0 : i + 1;

        FVector2D P0 = Polygon[i];
        FVector2D P1 = Polygon[j];

        Area += P0.X * P1.Y - P1.X * P0.Y;
    }

    return 0.5f * Area;
}