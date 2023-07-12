#pragma once
#include "RenderTarget.h"
#include "SceneView.h"
#include "D3D12/D3D12RHI.h"

class CSceneCapture2D
{
public:
    CSceneCapture2D(bool depthStencil, UINT width, UINT height, DXGI_FORMAT format, CD3D12RHI* d3d12RHI);

    CRenderTarget2D* GetRT() { return RT.get(); }

    const SSceneView& GetSceneView() const
    {
        return SceneView;
    }

    D3D12_VIEWPORT GetViewport() { return Viewport; }

    D3D12_RECT GetScissorRect() { return ScissorRect; }

    void CreatePerspectiveView(const SVector3& eye, const SVector3& target, const SVector3& up,
                               float fov, float aspectRatio, float nearPlane, float farPlane);

    void CreateOrthographicView(const SVector3& eye, const SVector3 target, const SVector3& up,
                                float left, float right, float bottom, float top, float nearPlane, float farPlane);

    UINT GetWidth() { return Width; }

    UINT GetHeight() { return Height; }

private:
    void SetViewportAndScissorRect();

private:
    CD3D12RHI* D3D12RHI = nullptr;

    UINT Width;

    UINT Height;

    std::unique_ptr<CRenderTarget2D> RT = nullptr;

    SSceneView SceneView;

    D3D12_VIEWPORT Viewport;

    D3D12_RECT ScissorRect;
};

class CSceneCaptureCube
{
public:
    CSceneCaptureCube(bool depthStencil, UINT size, DXGI_FORMAT format, CD3D12RHI* d3d12RHI);

    CRenderTargetCube* GetRTCube() { return RTCube.get(); }

    const SSceneView& GetSceneView(UINT Index) const
    {
        return SceneViews[Index];
    }

    D3D12_VIEWPORT GetViewport() { return Viewport; }

    D3D12_RECT GetScissorRect() { return ScissorRect; }

    void CreatePerspectiveViews(const SVector3& eye, float nearPlane, float farPlane);

    UINT GetCubeMapSize() { return CubeMapSize; }

private:

    void SetViewportAndScissorRect(UINT cubeMapSize);

private:
    CD3D12RHI* D3D12RHI = nullptr;

    UINT CubeMapSize;

    std::unique_ptr<CRenderTargetCube> RTCube = nullptr;

    SSceneView SceneViews[6];

    D3D12_VIEWPORT Viewport;

    D3D12_RECT ScissorRect;
};