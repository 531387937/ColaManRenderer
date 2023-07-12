#pragma once
#include "D3D12/D3D12Buffer.h"

struct SPrimitiveBatch
{
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

    CD3D12VertexBufferRef VertexBufferRef = nullptr;

    int CurrentVertexNum = 0;
};
