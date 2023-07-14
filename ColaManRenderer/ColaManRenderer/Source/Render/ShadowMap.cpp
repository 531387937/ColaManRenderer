#include "ShadowMap.h"

CShadowMap2D::CShadowMap2D(UINT width, UINT height, DXGI_FORMAT format, CD3D12RHI* d3d12RHI): CSceneCapture2D(
    true, width, height, format, d3d12RHI)
{
}

CShadowMapCube::CShadowMapCube(UINT size, DXGI_FORMAT format, CD3D12RHI* d3d12RHI): CSceneCaptureCube(
    true, size, format, d3d12RHI)
{
}
