#pragma once

#include "D3D12CommandContext.h"
#include "D3D12MemoryAllocator.h"
#include "D3D12HeapSlotAllocator.h"

class CD3D12RHI;

class CD3D12Device
{
public:
    CD3D12Device(CD3D12RHI* d3d12RHI);

    ~CD3D12Device();

    ID3D12Device* GetD3DDevice() { return D3DDevice.Get(); }

    CD3D12CommandContext* GetCommandContext() { return CommandContext.get(); }

    ID3D12CommandQueue* GetCommandQueue() { return CommandContext->GetCommandQueue(); }

    ID3D12GraphicsCommandList* GetCommandList() { return CommandContext->GetCommandList(); }

    CD3D12UploadBufferAllocator* GetUploadBufferAllocator() { return UploadBufferAllocator.get(); }

    CD3D12DefaultBufferAllocator* GetDefaultBufferAllocator() { return DefaultBufferAllocator.get(); }

    CD3D3TextureResourceAllocator* GetTextureResourceAllocator() { return TextureResourceAllocator.get(); }

    CD3D12HeapSlotAllocator* GetHeapSlotAllocator(D3D12_DESCRIPTOR_HEAP_TYPE HeapType);

private:
    void Initialize();

private:
    CD3D12RHI* D3D12RHI = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Device> D3DDevice = nullptr;

    std::unique_ptr<CD3D12CommandContext> CommandContext = nullptr;

private:
    std::unique_ptr<CD3D12UploadBufferAllocator> UploadBufferAllocator = nullptr;

    std::unique_ptr<CD3D12DefaultBufferAllocator> DefaultBufferAllocator = nullptr;

    std::unique_ptr<CD3D3TextureResourceAllocator> TextureResourceAllocator = nullptr;

    std::unique_ptr<CD3D12HeapSlotAllocator> RTVHeapSlotAllocator = nullptr;

    std::unique_ptr<CD3D12HeapSlotAllocator> DSVHeapSlotAllocator = nullptr;

    std::unique_ptr<CD3D12HeapSlotAllocator> SRVHeapSlotAllocator = nullptr;
};
