#include "D3D12CommandContext.h"

#include "D3D12DescriptorCache.h"
#include "D3D12Device.h"

CD3D12CommandContext::CD3D12CommandContext(CD3D12Device* device):Device(device)
{
    CreateCommandContext();

    DescriptorCache = std::make_unique<CD3D12DescriptorCache>(Device);
}

CD3D12CommandContext::~CD3D12CommandContext()
{
    DestroyCommandContext();
}

void CD3D12CommandContext::CreateCommandContext()
{
    ThrowIfFailed(Device->GetD3DDevice()->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&Fence)));

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(Device->GetD3DDevice()->CreateCommandQueue(&queueDesc,IID_PPV_ARGS(&CommandQueue)));

    ThrowIfFailed(Device->GetD3DDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(CommandListAlloc.GetAddressOf())));

    ThrowIfFailed(Device->GetD3DDevice()->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,CommandListAlloc.Get(),nullptr,IID_PPV_ARGS(CommandList.GetAddressOf())));

    ThrowIfFailed(CommandList->Close());
}

void CD3D12CommandContext::DestroyCommandContext()
{
}

void CD3D12CommandContext::ResetCommandAllocator()
{
    ThrowIfFailed(CommandListAlloc->Reset());
}

void CD3D12CommandContext::ResetCommandList()
{
    ThrowIfFailed(CommandList->Reset(CommandListAlloc.Get(),nullptr));
}

void CD3D12CommandContext::ExecuteCommandLists()
{
    ThrowIfFailed(CommandList->Close());

    ID3D12CommandList* cmdsLists[] = {CommandList.Get()};
    CommandQueue->ExecuteCommandLists(_countof(cmdsLists),cmdsLists);
}

void CD3D12CommandContext::FlushCommandQueue()
{
    CurrentFenceValue++;
	
    // Add an instruction to the command queue to set a new fence point.  
    // Because we are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    ThrowIfFailed(CommandQueue->Signal(Fence.Get(), CurrentFenceValue));
	
    // Wait until the GPU has completed commands up to this fence point.
    if (Fence->GetCompletedValue() < CurrentFenceValue)
    {
        HANDLE eventHandle = CreateEvent(nullptr, false, false, nullptr);
	
        // Fire event when GPU hits current fence.  
        ThrowIfFailed(Fence->SetEventOnCompletion(CurrentFenceValue, eventHandle));
	
        // Wait until the GPU hits current fence event is fired.
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void CD3D12CommandContext::EndFrame()
{
    DescriptorCache->Reset();
}
