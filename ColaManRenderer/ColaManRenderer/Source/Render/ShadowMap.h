#pragma once
#include "SceneCapture.h"

enum class EShadowMapType
{
    SM_SINGLE,
    SM_OMNI
};

enum class EShadowMapImpl
{
    PCF,
    PCSS,
    VSM,
    SDF
};

class CShadowMap2D:public CSceneCapture2D
{
public:
    CShadowMap2D(UINT width,UINT height,DXGI_FORMAT format,CD3D12RHI* d3d12RHI);
};

class CShadowMapCube:public CSceneCaptureCube
{
public:
    CShadowMapCube(UINT size,DXGI_FORMAT format,CD3D12RHI* d3d12RHI);
};
