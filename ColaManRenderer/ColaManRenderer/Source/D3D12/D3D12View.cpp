#include "D3D12View.h"

#include "D3D12Device.h"
#include <assert.h>

CD3D12View::CD3D12View(CD3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12Resource* resource): Device(device),
    Resource(resource), Type(type)
{
    HeapSlotAllocator = Device->GetHeapSlotAllocator(Type);

    if (HeapSlotAllocator)
    {
        HeapSlot = HeapSlotAllocator->AllocateHeapSlot();
        assert(HeapSlot.Handle.ptr!=0);
    }
}

CD3D12View::~CD3D12View()
{
    Destroy();
}

void CD3D12View::Destroy()
{
    if (HeapSlotAllocator)
    {
        HeapSlotAllocator->FreeHeapSlot(HeapSlot);
    }
}

CD3D12ShaderResourceView::CD3D12ShaderResourceView(CD3D12Device* device, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc,
                                                   ID3D12Resource* resource): CD3D12View(
    device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, resource)
{
    CreateShaderResourceView(desc);
}

CD3D12ShaderResourceView::~CD3D12ShaderResourceView()
{
}

void CD3D12ShaderResourceView::CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& desc)
{
    Device->GetD3DDevice()->CreateShaderResourceView(Resource, &desc, HeapSlot.Handle);
}

CD3D12RenderTargetView::CD3D12RenderTargetView(CD3D12Device* device, const D3D12_RENDER_TARGET_VIEW_DESC& desc,
                                               ID3D12Resource* resource): CD3D12View(
    device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, resource)
{
    CreateRenderTargetView(desc);
}

CD3D12RenderTargetView::~CD3D12RenderTargetView()
{
}

void CD3D12RenderTargetView::CreateRenderTargetView(const D3D12_RENDER_TARGET_VIEW_DESC& desc)
{
    Device->GetD3DDevice()->CreateRenderTargetView(Resource, &desc, HeapSlot.Handle);
}

CD3D12DepthStencilView::CD3D12DepthStencilView(CD3D12Device* device, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc,
                                               ID3D12Resource* resource): CD3D12View(
    device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, resource)
{
    CreateDepthStencilView(desc);
}

CD3D12DepthStencilView::~CD3D12DepthStencilView()
{
}

void CD3D12DepthStencilView::CreateDepthStencilView(const D3D12_DEPTH_STENCIL_VIEW_DESC& desc)
{
    Device->GetD3DDevice()->CreateDepthStencilView(Resource, &desc, HeapSlot.Handle);
}

CD3D12UnorderedAccessView::CD3D12UnorderedAccessView(CD3D12Device* device, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc,
                                                     ID3D12Resource* resource): CD3D12View(
    device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, resource)
{
    CreateUnorderedAccessView(desc);
}

CD3D12UnorderedAccessView::~CD3D12UnorderedAccessView()
{
}

void CD3D12UnorderedAccessView::CreateUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc)
{
    Device->GetD3DDevice()->CreateUnorderedAccessView(Resource, nullptr, &desc, HeapSlot.Handle);
}
