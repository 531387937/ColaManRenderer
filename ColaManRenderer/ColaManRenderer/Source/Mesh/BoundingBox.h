#pragma once
#include <DirectXCollision.h>
#include <vector>
#include "Vertex.h"
#include "Math/Transform.h"
#include "Mesh/Ray.h"

class CBoundingBox
{
public:
    void Init(std::vector<SVector3> Points);

    void Init(std::vector<SVertex> Vertices);

    SVector3 GetCenter() const { return (Min + Max) * 0.5f; }

    SVector3 GetExtend() const { return (Max - Min) * 0.5f; }

    SVector3 GetSize() const { return  (Max - Min); }

    int GetWidestAxis() const;

    float GetMaxWidth() const;

    float GetSurfaceArea() const;

    static CBoundingBox Union(const CBoundingBox& BoxA, const CBoundingBox& BoxB);

    static CBoundingBox Union(const CBoundingBox& Box, const SVector3& Point);

    CBoundingBox Transform(const CTransform& T);

    // If the ray’s origin is inside the box, 0 is returned for Dist0
    bool Intersect(const CRay& Ray, float& Dist0, float& Dist1);

    DirectX::BoundingBox GetD3DBox();
public:
    bool bInit = false;

    SVector3 Min = SVector3(CMath::Infinity);
    SVector3 Max = SVector3(-CMath::Infinity);
};
