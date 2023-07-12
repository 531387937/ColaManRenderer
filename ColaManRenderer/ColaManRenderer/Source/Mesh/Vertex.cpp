#include "Vertex.h"

SVertex::SVertex()
{
}

SVertex::SVertex(const SVector3& p, const SVector3& n, const SVector3& t, const SVector2& uv):
        Position(p),
        Normal(n),
        TangentU(t),
        TexCoord(uv)
{
}

SVertex::SVertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float u,
float v):Position(px, py, pz),
    Normal(nx, ny, nz),
    TangentU(tx, ty, tz),
    TexCoord(u, v)
{
}
