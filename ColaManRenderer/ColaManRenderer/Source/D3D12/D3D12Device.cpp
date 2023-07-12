#include "D3D12Device.h"

#include "D3D12RHI.h"


CD3D12Device::CD3D12Device(CD3D12RHI* d3d12RHI):D3D12RHI(d3d12RHI)
{
    Initialize();
}

CD3D12Device::~CD3D12Device()
{
}

CD3D12HeapSlotAllocator* CD3D12Device::GetHeapSlotAllocator(D3D12_DESCRIPTOR_HEAP_TYPE HeapType)
{
    switch (HeapType)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        return SRVHeapSlotAllocator.get();
        break;

        //case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        //	break;

        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
            return RTVHeapSlotAllocator.get();
        break;

    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
        return DSVHeapSlotAllocator.get();
        break;

    default:
        return nullptr;
    }
}

void CD3D12Device::Initialize()
{
    HRESULT HardwareResult = D3D12CreateDevice(nullptr,D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&D3DDevice));

    if(FAILED(HardwareResult))
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter> WarpAdapter;
        ThrowIfFailed(D3D12RHI->GetDxgiFactory()->EnumWarpAdapter(IID_PPV_ARGS(&WarpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(WarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&D3DDevice)));

        //Create CommandContext
        CommandContext = std::make_unique<CD3D12CommandContext>(this);

        //Create memory allocator
        UploadBufferAllocator = std::make_unique<TD3D12UploadBufferAllocator>(D3DDevice.Get());

        DefaultBufferAllocator = std::make_unique<TD3D12DefaultBufferAllocator>(D3DDevice.Get());

        TextureResourceAllocator = std::make_unique<CD3D3TextureResourceAllocator>(D3DDevice.Get());

        //Create heapSlot allocator
        RTVHeapSlotAllocator = std::make_unique<CD3D12HeapSlotAllocator>(D3DDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 200);

        DSVHeapSlotAllocator = std::make_unique<CD3D12HeapSlotAllocator>(D3DDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 200);

        SRVHeapSlotAllocator = std::make_unique<CD3D12HeapSlotAllocator>(D3DDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 200);
    }
}
