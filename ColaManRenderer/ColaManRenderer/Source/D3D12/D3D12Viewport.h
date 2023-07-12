#pragma once
#include "D3D12Utils.h"
#include "D3D12Texture.h"

class CD3D12RHI;

struct SD3D12ViewportInfo
{
    HWND WindowHandle;

    DXGI_FORMAT BackBufferFormat;
    DXGI_FORMAT DepthStencilFormat;

    bool bEnable4xMsaa = false;
    UINT QualityOf4xMsaa = 0;
};

class CD3D12Viewport
{
public:
    CD3D12Viewport(CD3D12RHI* d3d12RHI, const SD3D12ViewportInfo& info, int width, int height);

    ~CD3D12Viewport();

    void OnResize(int width, int height);

    void GetD3DViewport(D3D12_VIEWPORT& d3dViewPort, D3D12_RECT& d3dRect);

    void Present();

    CD3D12Resource* GetCurrentBackBuffer() const;

    CD3D12RenderTargetView* GetCurrentBackBufferView() const;

    float* GetCurrentBackBufferClearColor() const;

    CD3D12DepthStencilView* GetDepthStencilView() const;

    CD3D12ShaderResourceView* GetDepthShaderResourceView() const;

    SD3D12ViewportInfo GetViewportInfo() const;

private:
    void Initialize();

    void CreateSwapChain();

private:

    CD3D12RHI* D3D12RHI = nullptr;

    SD3D12ViewportInfo ViewportInfo;

    int ViewportWidth = 0;
    int ViewportHeight = 0;

    static const int SwapChainBufferCount = 2;
    Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain = nullptr;
    int CurrBackBuffer = 0;

    CD3D12TextureRef RenderTargetTextures[SwapChainBufferCount];

    CD3D12TextureRef DepthStencilTexture = nullptr;
};
