#include "BoundingBox.h"

void CBoundingBox::Init(std::vector<SVector3> Points)
{
    if (Points.size() > 0)
    {
        bInit = true;

        for (const SVector3& Point : Points)
        {
            Min = SVector3::Min(Min, Point);
            Max = SVector3::Max(Max, Point);
        }
    }
}

void CBoundingBox::Init(std::vector<SVertex> Vertices)
{
    if (Vertices.size() > 0)
    {
        bInit = true;

        for (const SVertex& Vertex : Vertices)
        {
            SVector3 P = Vertex.Position;

            Min = SVector3::Min(Min, P);
            Max = SVector3::Max(Max, P);
        }
    }
}

int CBoundingBox::GetWidestAxis() const
{
    SVector3 V = Max - Min;

    if (V.x > V.y && V.x > V.z)
        return 0;
    else if (V.y > V.z)
        return 1;
    else
        return 2;
}

float CBoundingBox::GetMaxWidth() const
{
    SVector3 V = Max - Min;

    if (V.x > V.y && V.x > V.z)
        return V.x;
    else if (V.y > V.z)
        return V.y;
    else
        return V.z;
}

float CBoundingBox::GetSurfaceArea() const
{
    if (bInit)
    {
        SVector3 V = Max - Min;

        return 2.0f * (V.x * V.y + V.x * V.z + V.y * V.z);
    }
    else
    {
        return 0.0f;
    }
}

CBoundingBox CBoundingBox::Union(const CBoundingBox& BoxA, const CBoundingBox& BoxB)
{
    CBoundingBox UnionBox;

    if (BoxA.bInit || BoxB.bInit)
    {
        UnionBox.bInit = true;

        UnionBox.Min = SVector3::Min(BoxA.Min, BoxB.Min);
        UnionBox.Max = SVector3::Max(BoxA.Max, BoxB.Max);
    }

    return UnionBox;
}

CBoundingBox CBoundingBox::Union(const CBoundingBox& Box, const SVector3& Point)
{
    CBoundingBox UnionBox;

    UnionBox.bInit = true;

    UnionBox.Min = SVector3::Min(Box.Min, Point);
    UnionBox.Max = SVector3::Max(Box.Max, Point);

    return UnionBox;
}

CBoundingBox CBoundingBox::Transform(const CTransform& T)
{
    CBoundingBox Box;

    if (bInit)
    {
        Box.bInit = true;

        // Transform eight corner points, and calculate new AABB
        SMatrix M = T.GetTransformMatrix();

        Box = Union(Box, M.Transform(SVector3(Min.x, Min.y, Min.z)));
        Box = Union(Box, M.Transform(SVector3(Max.x, Min.y, Min.z)));
        Box = Union(Box, M.Transform(SVector3(Min.x, Max.y, Min.z)));
        Box = Union(Box, M.Transform(SVector3(Min.x, Min.y, Max.z)));

        Box = Union(Box, M.Transform(SVector3(Min.x, Max.y, Max.z)));
        Box = Union(Box, M.Transform(SVector3(Max.x, Max.y, Min.z)));
        Box = Union(Box, M.Transform(SVector3(Max.x, Min.y, Max.z)));
        Box = Union(Box, M.Transform(SVector3(Max.x, Max.y, Max.z)));
    }

    return Box;
}

bool CBoundingBox::Intersect(const CRay& Ray, float& Dist0, float& Dist1)
{
    float t0 = 0, t1 = Ray.MaxDist;
    for (int i = 0; i < 3; ++i)
    {
        // Update interval for i'th bounding box slab
        float InvRayDir = 1.0f / Ray.Direction[i];
        float tNear = (Min[i] - Ray.Origin[i]) * InvRayDir;
        float tFar = (Max[i] - Ray.Origin[i]) * InvRayDir;

        // Update parametric interval from slab intersection t values
        if (tNear > tFar) std::swap(tNear, tFar);

        // Update tFar to ensure robust ray--bounds intersection
        tFar *= 1 + 2 * CMath::gamma(3);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) return false;
    }

    Dist0 = t0;
    Dist1 = t1;

    return true;
}

DirectX::BoundingBox CBoundingBox::GetD3DBox()
{
    DirectX::BoundingBox D3DBox;

    D3DBox.Center = (Min + Max) * 0.5f;
    D3DBox.Extents = (Max - Min) * 0.5f;

    return D3DBox;
}
