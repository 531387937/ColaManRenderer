#include "D3D12Viewport.h"
#include "D3D12RHI.h"

CD3D12Viewport::CD3D12Viewport(CD3D12RHI* d3d12RHI, const SD3D12ViewportInfo& info, int width, int height):
    D3D12RHI(d3d12RHI), ViewportInfo(info), ViewportWidth(width), ViewportHeight(height)
{
    Initialize();
}

CD3D12Viewport::~CD3D12Viewport()
{
}

void CD3D12Viewport::OnResize(int width, int height)
{
    ViewportHeight = height;
    ViewportWidth = width;

    D3D12RHI->GetDevice()->GetCommandContext()->FlushCommandQueue();

    D3D12RHI->GetDevice()->GetCommandContext()->ResetCommandList();

    for (UINT i = 0; i < SwapChainBufferCount; i++)
    {
        RenderTargetTextures[i].reset();
    }
    DepthStencilTexture.reset();

    ThrowIfFailed(
        SwapChain->ResizeBuffers(SwapChainBufferCount,ViewportWidth,ViewportHeight,ViewportInfo.BackBufferFormat,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

    CurrBackBuffer = 0;

    for (UINT i = 0; i < SwapChainBufferCount; i++)
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> SwapChainBuffer = nullptr;
        ThrowIfFailed(SwapChain->GetBuffer(i,IID_PPV_ARGS(&SwapChainBuffer)));

        D3D12_RESOURCE_DESC backBufferDesc = SwapChainBuffer->GetDesc();

        STextureInfo textureInfo;
        textureInfo.RTVFormat = backBufferDesc.Format;
        textureInfo.InitState = D3D12_RESOURCE_STATE_PRESENT;
        RenderTargetTextures[i] = D3D12RHI->CreateTexture(SwapChainBuffer, textureInfo, TexCreate_RTV);
    }

    STextureInfo textureInfo;
    textureInfo.Type = ETextureType::TEXTURE_2D;
    textureInfo.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureInfo.Width = ViewportWidth;
    textureInfo.Height = ViewportHeight;
    textureInfo.Depth = 1;
    textureInfo.MipCount = 1;
    textureInfo.ArraySize = 1;
    textureInfo.InitState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    textureInfo.Format = DXGI_FORMAT_R24G8_TYPELESS; // Create with a typeless format, support DSV and SRV(for SSAO)
    textureInfo.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    textureInfo.SRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

    DepthStencilTexture = D3D12RHI->CreateTexture(textureInfo, TexCreate_DSV | TexCreate_SRV);


    // Execute the resize commands.
    D3D12RHI->GetDevice()->GetCommandContext()->ExecuteCommandLists();

    // Wait until resize is complete.
    D3D12RHI->GetDevice()->GetCommandContext()->FlushCommandQueue();
}

void CD3D12Viewport::GetD3DViewport(D3D12_VIEWPORT& d3dViewPort, D3D12_RECT& d3dRect)
{
    d3dViewPort.TopLeftX = 0;
    d3dViewPort.TopLeftY = 0;
    d3dViewPort.Width = static_cast<float>(ViewportWidth);
    d3dViewPort.Height = static_cast<float>(ViewportHeight);
    d3dViewPort.MinDepth = 0.0f;
    d3dViewPort.MaxDepth = 1.0f;

    d3dRect = {0, 0, ViewportWidth, ViewportHeight};
}

void CD3D12Viewport::Present()
{
    ThrowIfFailed(SwapChain->Present(0, 0));
    CurrBackBuffer = (CurrBackBuffer + 1) % SwapChainBufferCount;
}

CD3D12Resource* CD3D12Viewport::GetCurrentBackBuffer() const
{
    return RenderTargetTextures[CurrBackBuffer]->GetResource();
}

CD3D12RenderTargetView* CD3D12Viewport::GetCurrentBackBufferView() const
{
    return RenderTargetTextures[CurrBackBuffer]->GetRTV();
}

float* CD3D12Viewport::GetCurrentBackBufferClearColor() const
{
    return RenderTargetTextures[CurrBackBuffer]->GetRTVClearValuePtr();
}

CD3D12DepthStencilView* CD3D12Viewport::GetDepthStencilView() const
{
    return DepthStencilTexture->GetDSV();
}

CD3D12ShaderResourceView* CD3D12Viewport::GetDepthShaderResourceView() const
{
    return DepthStencilTexture->GetSRV();
}

SD3D12ViewportInfo CD3D12Viewport::GetViewportInfo() const
{
    return ViewportInfo;
}

void CD3D12Viewport::Initialize()
{
    CreateSwapChain();
}

void CD3D12Viewport::CreateSwapChain()
{
    SwapChain.Reset();

    DXGI_SWAP_CHAIN_DESC desc;
    desc.BufferDesc.Width = ViewportWidth;
    desc.BufferDesc.Height = ViewportHeight;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format = ViewportInfo.BackBufferFormat;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.SampleDesc.Count = ViewportInfo.bEnable4xMsaa ? 4 : 1;
    desc.SampleDesc.Quality = ViewportInfo.bEnable4xMsaa ? (ViewportInfo.QualityOf4xMsaa - 1) : 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = SwapChainBufferCount;
    desc.OutputWindow = ViewportInfo.WindowHandle;
    desc.Windowed = true;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue = D3D12RHI->GetDevice()->GetCommandQueue();

    ThrowIfFailed(D3D12RHI->GetDxgiFactory()->CreateSwapChain(CommandQueue.Get(),&desc,SwapChain.GetAddressOf()));
}
