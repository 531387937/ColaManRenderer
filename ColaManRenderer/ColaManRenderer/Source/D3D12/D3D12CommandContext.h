#pragma once


#include "D3D12Utils.h"

class CD3D12DescriptorCache;

class CD3D12Device;

class CD3D12CommandContext
{
public:
    CD3D12CommandContext(CD3D12Device* device);

    ~CD3D12CommandContext();

    void CreateCommandContext();

    void DestroyCommandContext();

    ID3D12CommandQueue* GetCommandQueue() {return CommandQueue.Get();}

    ID3D12GraphicsCommandList* GetCommandList() {return CommandList.Get();}

    CD3D12DescriptorCache* GetDescriptorCache() { return DescriptorCache.get();}

    void ResetCommandAllocator();

    void ResetCommandList();

    void ExecuteCommandLists();

    void FlushCommandQueue();

    void EndFrame();

private:

    CD3D12Device* Device = nullptr;
    
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue = nullptr;

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandListAlloc = nullptr;
    
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList = nullptr;

    std::unique_ptr<CD3D12DescriptorCache> DescriptorCache = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Fence> Fence = nullptr;

    UINT64 CurrentFenceValue = 0;
};
