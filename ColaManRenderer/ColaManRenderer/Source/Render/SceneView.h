#pragma once
#include "D3D12/D3D12Utils.h"
#include "Math/Math.h"

struct SSceneView
{
    SVector3 EyePos = SVector3::Zero;
    SMatrix View = SMatrix::Identity;
    SMatrix Proj = SMatrix::Identity;

    float Near;
    float Far;
};