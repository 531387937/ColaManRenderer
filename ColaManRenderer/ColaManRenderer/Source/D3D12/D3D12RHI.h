#pragma once
#include "D3D12Buffer.h"
#include "D3D12Device.h"
#include "D3D12Texture.h"
#include "D3D12Viewport.h"
#include "Math/Math.h"
#include "Texture/TextureInfo.h"

class CD3D12RHI
{
public:
    CD3D12RHI();

    ~CD3D12RHI();

    void Initialize(HWND windowHandle, int windowWidth, int windowHeight);

    void Destroy();

    CD3D12Device* GetDevice() {return Device.get();}

    CD3D12Viewport* GetViewport() {return Viewport.get();}

    const SD3D12ViewportInfo& GetViewportInfo();

    IDXGIFactory4* GetDxgiFactory();
    //////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////// RHI CommandList //////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    void FlushCommandQueue();

    void ExecuteCommandLists();

    void ResetCommandList();

    void ResetCommandAllocator();

    void Present();

    void ResizeViewport(int width,int height);

    void TransitionResource(CD3D12Resource* resource,D3D12_RESOURCE_STATES stateAfter);

    void CopyResource(CD3D12Resource* dstResource, CD3D12Resource* srcResource);

    void CopyBufferRegion(CD3D12Resource* dstResource, UINT64 dstOffset,CD3D12Resource* srcResource, UINT64 srcOffset,UINT64 size);

    void CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION* dst, UINT dstX, UINT dstY, UINT dstZ, const D3D12_TEXTURE_COPY_LOCATION* src, const D3D12_BOX* srcBox);

    CD3D12ConstantBufferRef CreateConstantBuffer(const void* contents,uint32_t size);

    CD3D12StructuredBufferRef CreateStructuredBuffer(const void* contents, uint32_t elementSize,uint32_t elementCount);

    CD3D12RWStructuredBufferRef CreateRWStructuredBuffer(uint32_t elementSize,uint32_t elementCount);

    CD3D12VertexBufferRef CreateVertexBuffer(const void* contents,uint32_t size);

    CD3D12IndexBufferRef CreateIndexBuffer(const void* contents, uint32_t size);

    CD3D12ReadBackBufferRef CreateReadBackBuffer(uint32_t size);

    CD3D12TextureRef CreateTexture(const STextureInfo& textureInfo,uint32_t createFlags, SVector4 rtvClearValue = SVector4::Zero);

    CD3D12TextureRef CreateTexture(Microsoft::WRL::ComPtr<ID3D12Resource> d3dResource, STextureInfo& textureInfo, uint32_t createFlags);

    void UploadTextureData(CD3D12TextureRef texture, const std::vector<D3D12_SUBRESOURCE_DATA>& initData);

    void SetVertexBuffer(const CD3D12VertexBufferRef& vertexBuffer, UINT offset, UINT stride, UINT size);

    void SetIndexBuffer(const CD3D12IndexBufferRef& indexBuffer,UINT offset, DXGI_FORMAT format, UINT size);

    void EndFrame();

private:
    void CreateDefaultBuffer(uint32_t size, uint32_t alignment, D3D12_RESOURCE_FLAGS flags, CD3D12ResourceLocation& resourceLocation);

    void CreateAndInitDefaultBuffer(const void* contents, uint32_t size, uint32_t alignment,CD3D12ResourceLocation& resourceLocation);

    CD3D12TextureRef CreateTextureResource(const STextureInfo& textureInfo, uint32_t createFlags, SVector4 rtvClearValue);

    void  CreateTextureViews(CD3D12TextureRef TextureRef, const STextureInfo& TextureInfo, uint32_t CreateFlags);

    void LogAdapters();

    void LogAdapterOutputs(IDXGIAdapter* adapter);

    void LogOutputDisplayModes(IDXGIOutput* output,DXGI_FORMAT format);

    UINT GetSupportMSAAQuality(DXGI_FORMAT backBufferFormat);
private:
    std::unique_ptr<CD3D12Device> Device = nullptr;

    std::unique_ptr<CD3D12Viewport> Viewport = nullptr;

    SD3D12ViewportInfo ViewportInfo;

    Microsoft::WRL::ComPtr<IDXGIFactory4> DxgiFactory = nullptr;
};
