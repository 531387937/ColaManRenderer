#pragma once
#include "D3D12HeapSlotAllocator.h"

class CD3D12Device;


class CD3D12View
{
public:
    CD3D12View(CD3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Resource* resource);

    virtual ~CD3D12View();

    D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const { return HeapSlot.Handle; }

private:
    void Destroy();

protected:
    CD3D12Device* Device = nullptr;

    CD3D12HeapSlotAllocator* HeapSlotAllocator = nullptr;

    ID3D12Resource* Resource = nullptr;

    CD3D12HeapSlotAllocator::HeapSlot HeapSlot;

    D3D12_DESCRIPTOR_HEAP_TYPE Type;
};

class CD3D12ShaderResourceView : public CD3D12View
{
public:
    CD3D12ShaderResourceView(CD3D12Device* device, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc,
                             ID3D12Resource* resource);

    ~CD3D12ShaderResourceView() override;

protected:
    void CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);
};

class CD3D12RenderTargetView : public CD3D12View
{
public:
    CD3D12RenderTargetView(CD3D12Device* device, const D3D12_RENDER_TARGET_VIEW_DESC& desc, ID3D12Resource* resource);

    ~CD3D12RenderTargetView() override;

protected:
    void CreateRenderTargetView(const D3D12_RENDER_TARGET_VIEW_DESC& desc);
};

class CD3D12DepthStencilView : public CD3D12View
{
public:
    CD3D12DepthStencilView(CD3D12Device* device, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc, ID3D12Resource* resource);

    ~CD3D12DepthStencilView() override;

protected:
    void CreateDepthStencilView(const D3D12_DEPTH_STENCIL_VIEW_DESC& desc);
};

class CD3D12UnorderedAccessView : public CD3D12View
{
public:
    CD3D12UnorderedAccessView(CD3D12Device* device, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc,
                              ID3D12Resource* resource);

    ~CD3D12UnorderedAccessView() override;

protected:
    void CreateUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& Desc);
};
