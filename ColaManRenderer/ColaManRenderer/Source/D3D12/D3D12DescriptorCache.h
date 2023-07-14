#pragma once
#include "D3D12Utils.h"
#include "vector"
class CD3D12Device;

class CD3D12DescriptorCache
{
public:
    CD3D12DescriptorCache(CD3D12Device* device);

    ~CD3D12DescriptorCache();

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCacheCbvSrvUavDescriptorHeap()
    {
        return CacheCbvSrvUavDescriptorHeap;
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE AppendCbvSrvUavDescriptors(
        const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& descriptors);

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetCacheRtvDescriptorHeap() { return CacheRtvDescriptorHeap; }

    void AppendRtvDescriptors(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& rtvDescriptors,
                              CD3DX12_GPU_DESCRIPTOR_HANDLE& outGPUHandle,
                              CD3DX12_CPU_DESCRIPTOR_HANDLE& outCPUHandle);

    void Reset();

private:
    void CreateCacheCbvSrvUavDescriptorHeap();

    void CreateCacheRtvDescriptorHeap();

    void ResetCacheCbvSrvUavDescriptorHeap();

    void ResetCacheRtvDescriptorHeap();

private:
    CD3D12Device* Device = nullptr;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CacheCbvSrvUavDescriptorHeap = nullptr;

    UINT CbvSrvUavDescriptorSize;

    static const int MaxCbvSrvUavDescripotrCount = 2048;

    uint32_t CbvSrvUavDescriptorOffset = 0;

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CacheRtvDescriptorHeap = nullptr;

    UINT RtvDescriptorSize;

    static const int MaxRtvDescriptorCount = 1024;

    uint32_t RtvDescriptorOffset = 0;
};
