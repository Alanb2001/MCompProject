#include "Geom.h"

bool Geom::AreCoincident(const FVector2D& a, const FVector2D& b)
{
    return (a - b).Size() < 0.000001f;
}

bool Geom::ToTheLeft(const FVector2D& p, const FVector2D& l0, const FVector2D& l1)
{
    return ((l1.X - l0.X) * (p.Y - l0.Y) - (l1.Y - l0.Y) * (p.X - l0.X)) >= 0;
}

bool Geom::ToTheRight(const FVector2D& p, const FVector2D& l0, const FVector2D& l1)
{
    return !ToTheLeft(p, l0, l1);
}

bool Geom::PointInTriangle(const FVector2D& p, const FVector2D& c0, const FVector2D& c1, const FVector2D& c2)
{
    return ToTheLeft(p, c0, c1)
        && ToTheLeft(p, c1, c2)
        && ToTheLeft(p, c2, c0);
}

bool Geom::InsideCircumcircle(const FVector2D& p, const FVector2D& c0, const FVector2D& c1, const FVector2D& c2)
{
    float ax = c0.X - p.X;
    float ay = c0.Y - p.Y;
    float bx = c1.X - p.X;
    float by = c1.Y - p.Y;
    float cx = c2.X - p.X;
    float cy = c2.Y - p.Y;

    float det = (ax * ax + ay * ay) * (bx * cy - cx * by)
        - (bx * bx + by * by) * (ax * cy - cx * ay)
        + (cx * cx + cy * cy) * (ax * by - bx * ay);

    return det > 0.000001f;
}

FVector2D Geom::RotateRightAngle(const FVector2D& v)
{
    float x = v.X;
    float y = v.Y;
    return FVector2D(-y, x);
}

bool Geom::LineLineIntersection(FVector2D p0, FVector2D v0, FVector2D p1, FVector2D v1, float& m0, float& m1)
{
    float det = (v0.X * v1.Y - v0.Y * v1.X);

    if (FMath::Abs(det) < 0.001f)
    {
        m0 = NAN;
        m1 = NAN;
        return false;
    }
    else
    {
        m0 = ((p0.Y - p1.Y) * v1.X - (p0.X - p1.X) * v1.Y) / det;

        if (FMath::Abs(v1.X) >= 0.001f)
        {
            m1 = (p0.X + m0 * v0.X - p1.X) / v1.X;
        }
        else
        {
            m1 = (p0.Y + m0 * v0.Y - p1.Y) / v1.Y;
        }

        return true;
    }
}

FVector2D Geom::LineLineIntersection(FVector2D p0, FVector2D v0, FVector2D p1, FVector2D v1)
{
    float m0, m1;

    if (LineLineIntersection(p0, v0, p1, v1, m0, m1)) 
    {
        return p0 + m0 * v0;
    }
    else 
    {
        return FVector2D(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
    }
}

FVector2D Geom::CircumcircleCenter(FVector2D c0, FVector2D c1, FVector2D c2)
{
    FVector2D mp0 = 0.5f * (c0 + c1);
    FVector2D mp1 = 0.5f * (c1 + c2);

    FVector2D v0 = RotateRightAngle(c0 - c1);
    FVector2D v1 = RotateRightAngle(c1 - c2);

    float m0, m1;

    LineLineIntersection(mp0, v0, mp1, v1, m0, m1);

    return mp0 + m0 * v0;
}

FVector2D Geom::TriangleCentroid(const FVector2D& c0, const FVector2D& c1, const FVector2D& c2)
{
    FVector2D val = (1.0f / 3.0f) * (c0 + c1 + c2);
    return val;
}

float Geom::Area(const TArray<FVector2D>& Polygon)
{
    float Area = 0.0f;
    int32 Count = Polygon.Num();

    for (int32 i = 0; i < Count; i++)
    {
        int32 j = (i == Count - 1) ? 0 : (i + 1);

        FVector2D P0 = Polygon[i];
        FVector2D P1 = Polygon[j];

        Area += P0.X * P1.Y - P1.X * P0.Y;
    }

    return 0.5f * Area;
}