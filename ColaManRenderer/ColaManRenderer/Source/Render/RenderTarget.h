#pragma once
#include "D3D12/D3D12RHI.h"

class CRenderTarget
{
public:
    CRenderTarget(CD3D12RHI* InD3D12RHI, bool RenderDepth, UINT InWidth, UINT InHeight, DXGI_FORMAT InFormat,
                  SVector4 InClearValue = SVector4::Zero);

    virtual ~CRenderTarget();

    CD3D12TextureRef GetTexture() const { return D3DTexture; }

    CD3D12Resource* GetResource() const { return D3DTexture->GetResource(); }

    DXGI_FORMAT GetFormat() const { return Format; }

    SVector4 GetClearColor() const { return ClearColor; }

    float* GetClearColorPtr() { return (float*)&ClearColor; }

protected:
    bool isDepthStencil = false;

    CD3D12RHI* D3D12RHI = nullptr;

    CD3D12TextureRef D3DTexture = nullptr;

    UINT Width;

    UINT Height;

    DXGI_FORMAT Format;

    SVector4 ClearColor;
};

class CRenderTarget2D : public CRenderTarget
{
public:
    CRenderTarget2D(CD3D12RHI* d3d12RHI, bool depthStencil, UINT width, UINT height, DXGI_FORMAT format,
                    SVector4 clearColor = SVector4::Zero);

    CD3D12RenderTargetView* GetRTV() const;

    CD3D12DepthStencilView* GetDSV() const;

    CD3D12ShaderResourceView* GetSRV() const;

private:
    void CreateTexture();
};

class CRenderTargetCube : public CRenderTarget
{
public:
    CRenderTargetCube(CD3D12RHI* d3d12RHI, bool depthStencil, UINT size, DXGI_FORMAT format,
                      SVector4 clearColor = SVector4::Zero);

    CD3D12RenderTargetView* GetRTV(int index) const;

    CD3D12DepthStencilView* GetDSV(int index) const;

    CD3D12ShaderResourceView* GetSRV() const;

private:
    void CreateTexture();
};
