#include "RenderTarget.h"

CRenderTarget::CRenderTarget(CD3D12RHI* InD3D12RHI, bool RenderDepth, UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat,
                             SVector4 InClearValue): isDepthStencil(RenderDepth), D3D12RHI(InD3D12RHI), Width(InWidth),
                                                     Height(InHeight), Format(InFormat), ClearColor(InClearValue)
{
}

CRenderTarget::~CRenderTarget()
{
}

CRenderTarget2D::CRenderTarget2D(CD3D12RHI* d3d12RHI, bool depthStencil, UINT width, UINT height, DXGI_FORMAT format,
                                 SVector4 clearColor): CRenderTarget(d3d12RHI, depthStencil, width, height, format,
                                                                     clearColor)
{
    CreateTexture();
}

CD3D12RenderTargetView* CRenderTarget2D::GetRTV() const
{
    if (isDepthStencil)
        return nullptr;
    return D3DTexture->GetRTV();
}

CD3D12DepthStencilView* CRenderTarget2D::GetDSV() const
{
    if (isDepthStencil)
        return D3DTexture->GetDSV();
    return nullptr;
}

CD3D12ShaderResourceView* CRenderTarget2D::GetSRV() const
{
    return D3DTexture->GetSRV();
}

void CRenderTarget2D::CreateTexture()
{
    STextureInfo textureInfo;
    textureInfo.Type = ETextureType::TEXTURE_2D;
    textureInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureInfo.Width = Width;
    textureInfo.Height = Height;
    textureInfo.Depth = 1;
    textureInfo.MipCount = 1;
    textureInfo.ArraySize = 1;
    textureInfo.Format = Format;

    if (isDepthStencil)
    {
        textureInfo.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        textureInfo.SRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

        D3DTexture = D3D12RHI->CreateTexture(textureInfo, TexCreate_DSV | TexCreate_SRV);
    }
    else
    {
        D3DTexture = D3D12RHI->CreateTexture(textureInfo, TexCreate_RTV | TexCreate_SRV, ClearColor);
    }
}

CRenderTargetCube::CRenderTargetCube(CD3D12RHI* d3d12RHI, bool depthStencil, UINT size, DXGI_FORMAT format,
                                     SVector4 clearColor): CRenderTarget(
    d3d12RHI, depthStencil, size, size, format, clearColor)
{
    CreateTexture();
}

CD3D12RenderTargetView* CRenderTargetCube::GetRTV(int index) const
{
    if (isDepthStencil)
        return nullptr;
    return D3DTexture->GetRTV(index);
}

CD3D12DepthStencilView* CRenderTargetCube::GetDSV(int index) const
{
    if (isDepthStencil)
        return D3DTexture->GetDSV(index);
    return nullptr;
}

CD3D12ShaderResourceView* CRenderTargetCube::GetSRV() const
{
    return D3DTexture->GetSRV();
}

void CRenderTargetCube::CreateTexture()
{
    STextureInfo textureInfo;
    textureInfo.Type = ETextureType::TEXTURE_CUBE;
    textureInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureInfo.Width = Width;
    textureInfo.Height = Height;
    textureInfo.Depth = 1;
    textureInfo.MipCount = 1;
    textureInfo.ArraySize = 6;
    textureInfo.Format = Format;

    if (isDepthStencil)
    {
        textureInfo.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        textureInfo.SRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

        D3DTexture = D3D12RHI->CreateTexture(textureInfo, TexCreate_CubeDSV | TexCreate_SRV);
    }
    else
    {
        D3DTexture = D3D12RHI->CreateTexture(textureInfo, TexCreate_CubeRTV | TexCreate_SRV, ClearColor);
    }
}
