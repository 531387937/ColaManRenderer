#include "D3D12DescriptorCache.h"

#include "D3D12Device.h"

CD3D12DescriptorCache::CD3D12DescriptorCache(CD3D12Device* device): Device(device)
{
    CreateCacheCbvSrvUavDescriptorHeap();

    CreateCacheRtvDescriptorHeap();
}

CD3D12DescriptorCache::~CD3D12DescriptorCache()
{
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CD3D12DescriptorCache::AppendCbvSrvUavDescriptors(
    const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& descriptors)
{
    uint32_t SlotsNeeded = static_cast<uint32_t>(descriptors.size());
    assert(CbvSrvUavDescriptorOffset+SlotsNeeded<MaxCbvSrvUavDescripotrCount);

    auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        CacheCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), CbvSrvUavDescriptorOffset,
        CbvSrvUavDescriptorSize);

    Device->GetD3DDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotsNeeded, SlotsNeeded, descriptors.data(),
                                            nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    auto GpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
        CacheCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), CbvSrvUavDescriptorOffset,
        CbvSrvUavDescriptorSize);

    CbvSrvUavDescriptorOffset += SlotsNeeded;

    return GpuDescriptorHandle;
}

void CD3D12DescriptorCache::AppendRtvDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& rtvDescriptors,
                                                 CD3DX12_GPU_DESCRIPTOR_HANDLE& outGPUHandle,
                                                 CD3DX12_CPU_DESCRIPTOR_HANDLE& outCPUHandle)
{
    // Append to heap
    uint32_t SlotsNeeded = static_cast<uint32_t>(rtvDescriptors.size());
    assert(RtvDescriptorOffset + SlotsNeeded < MaxRtvDescriptorCount);

    auto CpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), RtvDescriptorOffset, RtvDescriptorSize);
    Device->GetD3DDevice()->CopyDescriptors(1, &CpuDescriptorHandle, &SlotsNeeded, SlotsNeeded, rtvDescriptors.data(),
                                            nullptr, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    outGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetGPUDescriptorHandleForHeapStart(),
                                                 RtvDescriptorOffset, RtvDescriptorSize);

    outCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CacheRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                                                 RtvDescriptorOffset, RtvDescriptorSize);

    // Increase descriptor offset
    RtvDescriptorOffset += SlotsNeeded;
}

void CD3D12DescriptorCache::Reset()
{
    ResetCacheRtvDescriptorHeap();

    ResetCacheCbvSrvUavDescriptorHeap();
}

void CD3D12DescriptorCache::CreateCacheRtvDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC RtvHeapDesc = {};
    RtvHeapDesc.NumDescriptors = MaxRtvDescriptorCount;
    RtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    RtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    ThrowIfFailed(Device->GetD3DDevice()->CreateDescriptorHeap(&RtvHeapDesc,IID_PPV_ARGS(&CacheRtvDescriptorHeap)));

    SetDebugName(CacheRtvDescriptorHeap.Get(), L"TD3D12DescriptorCache CacheCbvSrvUavDescriptorHeap");

    RtvDescriptorSize = Device->GetD3DDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void CD3D12DescriptorCache::ResetCacheCbvSrvUavDescriptorHeap()
{
    CbvSrvUavDescriptorOffset = 0;
}

void CD3D12DescriptorCache::ResetCacheRtvDescriptorHeap()
{
    RtvDescriptorOffset = 0;
}

void CD3D12DescriptorCache::CreateCacheCbvSrvUavDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC SrvHeapDesc = {};
    SrvHeapDesc.NumDescriptors = MaxCbvSrvUavDescripotrCount;
    SrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    SrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ThrowIfFailed(
        Device->GetD3DDevice()->CreateDescriptorHeap(&SrvHeapDesc,IID_PPV_ARGS(&CacheCbvSrvUavDescriptorHeap)));

    SetDebugName(CacheCbvSrvUavDescriptorHeap.Get(), L"TD3D12DescriptorCache CacheCbvSrvUavDescriptorHeap");

    CbvSrvUavDescriptorSize = Device->GetD3DDevice()->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
