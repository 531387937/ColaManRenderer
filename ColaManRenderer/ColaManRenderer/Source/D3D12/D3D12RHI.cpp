#include "D3D12RHI.h"
#define InstalledDebugLayers true

CD3D12RHI::CD3D12RHI()
{
}

CD3D12RHI::~CD3D12RHI()
{
    Destroy();
}

void CD3D12RHI::Initialize(HWND windowHandle, int windowWidth, int windowHeight)
{
    UINT DxgiFactoryFlags = 0;
#if (defined(DEBUG) || defined(_DEBUG)) && InstalledDebugLayers
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> DebugController;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(DebugController.GetAddressOf())));
        DebugController->EnableDebugLayer();
    }

    Microsoft::WRL::ComPtr<IDXGIInfoQueue> DxgiInfoQueue;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(DxgiInfoQueue.GetAddressOf()))))
    {
        DxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

        DxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
        DxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
    }

#endif

    ThrowIfFailed(CreateDXGIFactory2(DxgiFactoryFlags,IID_PPV_ARGS(DxgiFactory.GetAddressOf())));

    Device = std::make_unique<CD3D12Device>(this);

    ViewportInfo.WindowHandle = windowHandle;
    ViewportInfo.BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    ViewportInfo.DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ViewportInfo.bEnable4xMsaa = false;
    ViewportInfo.QualityOf4xMsaa = GetSupportMSAAQuality(ViewportInfo.BackBufferFormat);

    Viewport = std::make_unique<CD3D12Viewport>(this, ViewportInfo, windowWidth, windowHeight);

#ifdef _DEBUG
    LogAdapters();
#endif
}

void CD3D12RHI::Destroy()
{
    EndFrame();

    Viewport.reset();

    Device.reset();
}

void CD3D12RHI::FlushCommandQueue()
{
    GetDevice()->GetCommandContext()->FlushCommandQueue();
}

void CD3D12RHI::ExecuteCommandLists()
{
    GetDevice()->GetCommandContext()->ExecuteCommandLists();
}

void CD3D12RHI::ResetCommandList()
{
    GetDevice()->GetCommandContext()->ResetCommandList();
}

void CD3D12RHI::ResetCommandAllocator()
{
    GetDevice()->GetCommandContext()->ResetCommandAllocator();
}

void CD3D12RHI::Present()
{
    GetViewport()->Present();
}

void CD3D12RHI::ResizeViewport(int width, int height)
{
    GetViewport()->OnResize(width, height);
}

void CD3D12RHI::TransitionResource(CD3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter)
{
    D3D12_RESOURCE_STATES CurrState = resource->CurrentState;

    if (stateAfter != CurrState)
    {
        GetDevice()->GetCommandList()->ResourceBarrier(
            1, &CD3DX12_RESOURCE_BARRIER::Transition(resource->D3DResource.Get(), CurrState, stateAfter));

        resource->CurrentState = stateAfter;
    }
}

void CD3D12RHI::CopyResource(CD3D12Resource* dstResource, CD3D12Resource* srcResource)
{
    GetDevice()->GetCommandList()->CopyResource(dstResource->D3DResource.Get(), srcResource->D3DResource.Get());
}

void CD3D12RHI::CopyBufferRegion(CD3D12Resource* dstResource, UINT64 dstOffset, CD3D12Resource* srcResource,
                                 UINT64 srcOffset, UINT64 size)
{
    GetDevice()->GetCommandList()->CopyBufferRegion(dstResource->D3DResource.Get(), dstOffset,
                                                    srcResource->D3DResource.Get(), srcOffset, size);
}

void CD3D12RHI::CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION* dst, UINT dstX, UINT dstY, UINT dstZ,
                                  const D3D12_TEXTURE_COPY_LOCATION* src, const D3D12_BOX* srcBox)
{
    GetDevice()->GetCommandList()->CopyTextureRegion(dst, dstX, dstY, dstZ, src, srcBox);
}

void CD3D12RHI::SetVertexBuffer(const CD3D12VertexBufferRef& vertexBuffer, UINT offset, UINT stride, UINT size)
{
    const CD3D12ResourceLocation& ResourceLocation = vertexBuffer->ResourceLocation;
    CD3D12Resource* resource = ResourceLocation.UnderlyingResource;
    TransitionResource(resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER);

    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.BufferLocation = ResourceLocation.GPUVirtualAddress + offset;
    vbv.StrideInBytes = stride;
    vbv.SizeInBytes = size;
    GetDevice()->GetCommandList()->IASetVertexBuffers(0, 1, &vbv);
}

void CD3D12RHI::SetIndexBuffer(const CD3D12IndexBufferRef& indexBuffer, UINT offset, DXGI_FORMAT format, UINT size)
{
    const CD3D12ResourceLocation& ResourceLocation = indexBuffer->ResourceLocation;
    CD3D12Resource* Resource = ResourceLocation.UnderlyingResource;
    TransitionResource(Resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER);

    // Set vertex buffer
    D3D12_INDEX_BUFFER_VIEW IBV;
    IBV.BufferLocation = ResourceLocation.GPUVirtualAddress + offset;
    IBV.Format = format;
    IBV.SizeInBytes = size;
    GetDevice()->GetCommandList()->IASetIndexBuffer(&IBV);
}

void CD3D12RHI::EndFrame()
{
    // Clean memory allocations
    GetDevice()->GetUploadBufferAllocator()->CleanUpAllocations();

    GetDevice()->GetDefaultBufferAllocator()->CleanUpAllocations();

    GetDevice()->GetTextureResourceAllocator()->CleanUpAllocations();

    // CommandContext
    GetDevice()->GetCommandContext()->EndFrame();
}


void CD3D12RHI::LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter* adapter = nullptr;
    std::vector<IDXGIAdapter*> adapterList;
    while (DxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring text = L"***Adapter: ";
        text += desc.Description;
        text += L"\n";

        OutputDebugString(text.c_str());

        adapterList.push_back(adapter);

        ++i;
    }

    for (size_t i = 0; i < adapterList.size(); ++i)
    {
        LogAdapterOutputs(adapterList[i]);
        ReleaseCom(adapterList[i]);
    }
}

void CD3D12RHI::LogAdapterOutputs(IDXGIAdapter* adapter)
{
    UINT i = 0;
    IDXGIOutput* output = nullptr;
    while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);

        std::wstring text = L"***Output: ";
        text += desc.DeviceName;
        text += L"\n";
        OutputDebugString(text.c_str());

        LogOutputDisplayModes(output, ViewportInfo.BackBufferFormat);

        ReleaseCom(output);

        ++i;
    }
}

void CD3D12RHI::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
    UINT count = 0;
    UINT flags = 0;

    // Call with nullptr to get list count.
    output->GetDisplayModeList(format, flags, &count, nullptr);

    std::vector<DXGI_MODE_DESC> modeList(count);
    output->GetDisplayModeList(format, flags, &count, &modeList[0]);

    for (auto& x : modeList)
    {
        UINT n = x.RefreshRate.Numerator;
        UINT d = x.RefreshRate.Denominator;
        std::wstring text =
            L"Width = " + std::to_wstring(x.Width) + L" " +
            L"Height = " + std::to_wstring(x.Height) + L" " +
            L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
            L"\n";

        ::OutputDebugString(text.c_str());
    }
}

UINT CD3D12RHI::GetSupportMSAAQuality(DXGI_FORMAT backBufferFormat)
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = backBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    ThrowIfFailed(Device->GetD3DDevice()->CheckFeatureSupport(
        D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
        &msQualityLevels,
        sizeof(msQualityLevels)));

    UINT QualityOf4xMsaa = msQualityLevels.NumQualityLevels;
    assert(QualityOf4xMsaa > 0 && "Unexpected MSAA quality level.");

    return QualityOf4xMsaa;
}

const SD3D12ViewportInfo& CD3D12RHI::GetViewportInfo()
{
    return ViewportInfo;
}

IDXGIFactory4* CD3D12RHI::GetDxgiFactory()
{
    return DxgiFactory.Get();
}
