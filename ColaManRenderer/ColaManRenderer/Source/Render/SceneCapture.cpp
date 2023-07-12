#include "SceneCapture.h"

CSceneCapture2D::CSceneCapture2D(bool depthStencil, UINT width, UINT height, DXGI_FORMAT format, CD3D12RHI* d3d12RHI):Height(height),
Width(width),D3D12RHI(d3d12RHI)
{
    RT = std::make_unique<CRenderTarget2D>(d3d12RHI,depthStencil,width,height,format);

    SetViewportAndScissorRect();
}

void CSceneCapture2D::CreatePerspectiveView(const SVector3& eye, const SVector3& target, const SVector3& up, float fov,
    float aspectRatio, float nearPlane, float farPlane)
{
    SceneView.EyePos = eye;
    SceneView.View = SMatrix::CreateLookAt(eye, target, up);
    SceneView.Proj = SMatrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane);

    SceneView.Near = nearPlane;
    SceneView.Far = farPlane;
}

void CSceneCapture2D::CreateOrthographicView(const SVector3& eye, const SVector3 target, const SVector3& up, float left,
    float right, float bottom, float top, float nearPlane, float farPlane)
{
    SceneView.EyePos = eye;
    SceneView.View = SMatrix::CreateLookAt(eye, target, up);
    SceneView.Proj = SMatrix::CreateOrthographicOffCenter(left, right, bottom, top, nearPlane, farPlane);

    SceneView.Near = nearPlane;
    SceneView.Far = farPlane;
}

void CSceneCapture2D::SetViewportAndScissorRect()
{
    Viewport = {0.0f,0.0f,(float)Width,(float)Height,0.0f,1.0f};

    ScissorRect = {0,0,(int)(Viewport.Width),(int)(Viewport.Height)};
}

CSceneCaptureCube::CSceneCaptureCube(bool depthStencil, UINT size, DXGI_FORMAT format, CD3D12RHI* d3d12RHI):D3D12RHI(d3d12RHI), CubeMapSize(size)
{
    D3D12RHI = d3d12RHI;

    RTCube = std::make_unique<CRenderTargetCube>(D3D12RHI, depthStencil, CubeMapSize, format);

    SetViewportAndScissorRect(CubeMapSize);
}

void CSceneCaptureCube::CreatePerspectiveViews(const SVector3& eye, float nearPlane, float farPlane)
{
    SVector3 Targets[6] =
    {
        eye + SVector3(1.0f,  0.0f,  0.0f), // +X 
        eye + SVector3(-1.0f, 0.0f,  0.0f), // -X 
        eye + SVector3(0.0f,  1.0f,  0.0f), // +Y 
        eye + SVector3(0.0f,  -1.0f, 0.0f), // -Y 
        eye + SVector3(0.0f,  0.0f,  1.0f), // +Z 
        eye + SVector3(0.0f,  0.0f, -1.0f)  // -Z 
    };

    SVector3 Ups[6] =
    {
        {0.0f, 1.0f, 0.0f},  // +X 
        {0.0f, 1.0f, 0.0f},  // -X 
        {0.0f, 0.0f, -1.0f}, // +Y 
        {0.0f, 0.0f, +1.0f}, // -Y 
        {0.0f, 1.0f, 0.0f},	 // +Z 
        {0.0f, 1.0f, 0.0f}	 // -Z 
    };

    for (int i = 0; i < 6; ++i)
    {
        SceneViews[i].EyePos = eye;
        SceneViews[i].View = SMatrix::CreateLookAt(eye, Targets[i], Ups[i]);

        float Fov = 0.5f * CMath::Pi;
        float AspectRatio = 1.0f; //Square
        SceneViews[i].Proj = SMatrix::CreatePerspectiveFieldOfView(Fov, AspectRatio, nearPlane, farPlane);

        SceneViews[i].Near = nearPlane;
        SceneViews[i].Far = farPlane;
    }
}

void CSceneCaptureCube::SetViewportAndScissorRect(UINT cubeMapSize)
{
    Viewport = { 0.0f, 0.0f, (float)cubeMapSize, (float)cubeMapSize, 0.0f, 1.0f };

    ScissorRect = { 0, 0, (int)(Viewport.Width), (int)(Viewport.Height) };
}
