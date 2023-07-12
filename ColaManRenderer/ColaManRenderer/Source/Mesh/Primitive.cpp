#include "Primitive.h"

#include "Mesh.h"

bool CPrimitive::GetLocalBoundingBox(CBoundingBox& box) const
{
    if(BoundingBox.bInit)
    {
        box = BoundingBox;

        return true;
    }
    else
    {
        return false;
    }
}

bool CPrimitive::GetWorldBoundingBox(CBoundingBox& box) const
{
    CBoundingBox localBox;

    if(GetLocalBoundingBox(localBox))
    {
        box = localBox.Transform(WorldTransform);

        return true;
    }
    else
    {
        return false;
    }
}

void CLine::GenerateBoundingBox()
{
    std::vector<SVector3> points;
    points.push_back(PointA);
    points.push_back(PointB);

    BoundingBox.Init(points);
}

void CTriangle::GenerateBoundingBox()
{
    std::vector<SVector3> points;
    points.push_back(PointA);
    points.push_back(PointB);
    points.push_back(PointC);

    BoundingBox.Init(points);
}

bool CTriangle::Intersect(const CRay& ray, float& dist, bool& bBackFace)
{
    const float EPSILON = 1e-5f;

    SVector3 dir = ray.Direction;
    SVector3 orig = ray.Origin;
    SVector3 edge1 = PointB-PointA;
    SVector3 edge2 = PointC-PointA;

    SVector3 pVec = dir.Cross(edge2);

    float det = edge1.Dot(pVec);

    if(det>-EPSILON&&det<EPSILON)
    {
        return false;
    }

    float invDet = 1.0f/det;

    SVector3 tVec = orig-PointA;

    float u = tVec.Dot(pVec)*invDet;
    if(u<0.0f||u>1.0f)
    {
        return false;
    }

    // Prepare to test V parameter
    SVector3 QVec = tVec.Cross(edge1);

    // Calculate V parameter and test bounds
    float v = dir.Dot(QVec) * invDet;
    if (v < 0.0f || u + v > 1.0f)
    {
        return false;
    }

    // Calculate T
    float t = edge2.Dot(QVec) * invDet;

    if (t < 0.0f)
    {
        return false;
    }

    float TValue = std::abs(t);
    if(TValue > ray.MaxDist)
    {
        return false;
    }

    dist = TValue;
    bBackFace = det < 0.0f ? true : false;

    return true;
}

void CMeshPrimitive::GenerateBoundingBox()
{
    //TO DO
    //CMesh& Mesh = CMeshRepository::Get().MeshMap.at(MeshName);

    //BoundingBox = Mesh.GetBoundingBox();
}
