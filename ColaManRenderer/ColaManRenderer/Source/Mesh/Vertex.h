#pragma once
#include "Mesh/Color.h"
#include "Math/Math.h"

struct SVertex
{
public:
    SVertex();
    SVertex(const SVector3& p, const SVector3& n, const SVector3& t, const SVector2& uv);
    SVertex(float px, float py, float pz,
            float nx, float ny, float nz,
            float tx, float ty, float tz,
            float u, float v);

    SVector3 Position;
    SVector3 Normal;
    SVector3 TangentU;
    SVector2 TexCoord;
};

struct SPrimitiveVertex
{
    SPrimitiveVertex()
    {
    }

    SPrimitiveVertex(
        const SVector3& p,
        const SColor& c) :
        Position(p),
        Color(c)
    {
    }

    SPrimitiveVertex(
        float px, float py, float pz,
        float cr, float cg, float cb, float ca) :
        Position(px, py, pz),
        Color(cr, cg, cb, ca)
    {
    }

    SVector3 Position;
    SColor Color;
};

struct SSpriteVertex
{
    SSpriteVertex()
    {
    }

    SSpriteVertex(
        const SVector3& p,
        const SVector2& uv):
        Position(p),
        TexCoord(uv)
    {
    }

    SVector3 Position;
    SVector2 TexCoord;
};
