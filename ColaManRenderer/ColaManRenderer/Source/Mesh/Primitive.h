#pragma once

#include "Color.h"
#include "Mesh/BoundingBox.h"
#include "Mesh/Ray.h"
#include <string>

class CPrimitive
{
public:
    CPrimitive(){}

    virtual ~CPrimitive(){}

    virtual void GenerateBoundingBox(){}

    bool GetLocalBoundingBox(CBoundingBox& box) const;

    bool GetWorldBoundingBox(CBoundingBox& box) const;

    virtual bool Intersect(const CRay& ray, float& dist, bool& bBackFace) { return false; }
protected:
    CTransform WorldTransform;
    CBoundingBox BoundingBox;
};


class CPoint:public CPrimitive
{
public:
    CPoint() = default;

    CPoint(const SVector3& point, const SColor color):Color(color),Point(point){}
public:
    SVector3 Point;
    SColor Color;
};

class CLine:public CPrimitive
{
public:
    CLine() = default;

    CLine(const SVector3& pointA,const SVector3& pointB,const SColor color):PointA(pointA),PointB(pointB),Color(color){}

    virtual void GenerateBoundingBox() override;
public:
    SVector3 PointA;
    SVector3 PointB;
    SColor Color;
};

class CTriangle:public CPrimitive
{
public:
    CTriangle() = default;

    CTriangle(const SVector3& pointA,const SVector3& pointB,const SVector3& pointC,const SColor& color):PointA(pointA),
    PointB(pointB),PointC(pointC),Color(color){}

    virtual void GenerateBoundingBox() override;

    virtual bool Intersect(const CRay& ray, float& dist, bool& bBackFace) override;
public:
    SVector3 PointA;
    SVector3 PointB;
    SVector3 PointC;
    SColor Color;
};

class CMeshPrimitive:public CPrimitive
{
public:
    CMeshPrimitive(const std::string& meshName,const CTransform& worldTransform):MeshName(meshName)
    {
        WorldTransform = worldTransform;
    }

    virtual void GenerateBoundingBox() override;
private:
    std::string MeshName;
};