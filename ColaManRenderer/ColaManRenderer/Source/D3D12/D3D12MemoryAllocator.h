#pragma once

#include "D3D12Resource.h"
#include <stdint.h>
#include <set>
#include <vector>

#define DEFAULT_POOL_SIZE (512 * 1024 * 512)

#define DEFAULT_RESOURCE_ALIGNMENT 4
#define UPLOAD_RESOURCE_ALIGNMENT 256

class CD3D12BuddyAllocator
{
public:
    enum class EAllocationStrategy
    {
        PlacedResource,
        ManualSubAllocation
    };

    struct TAllocatorInitData
    {
        EAllocationStrategy AllocationStrategy;

        D3D12_HEAP_TYPE HeapType;

        D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE; // Only for PlacedResource

        D3D12_RESOURCE_FLAGS ResourceFlags = D3D12_RESOURCE_FLAG_NONE; // Only for ManualSubAllocation
    };

public:
    CD3D12BuddyAllocator(ID3D12Device* InDevice, const TAllocatorInitData& InInitData);

    ~CD3D12BuddyAllocator();

    bool AllocResource(uint32_t Size, uint32_t Alignment, CD3D12ResourceLocation& ResourceLocation);

    void Deallocate(CD3D12ResourceLocation& ResourceLocation);

    void CleanUpAllocations();

    ID3D12Heap* GetBackingHeap() { return BackingHeap; }

    EAllocationStrategy GetAllocationStrategy() { return InitData.AllocationStrategy; }

private:
    void Initialize();

    uint32_t AllocateBlock(uint32_t Order);

    uint32_t GetSizeToAllocate(uint32_t Size, uint32_t Alignment);

    bool CanAllocate(uint32_t SizeToAllocate);

    uint32_t SizeToUnitSize(uint32_t Size) const
    {
        return (Size + (MinBlockSize - 1)) / MinBlockSize;
    }

    uint32_t UnitSizeToOrder(uint32_t Size) const
    {
        unsigned long Result;
        _BitScanReverse(&Result, Size + Size - 1); // ceil(log2(size))
        return Result;
    }

    uint32_t OrderToUnitSize(uint32_t Order) const
    {
        return static_cast<uint32_t>(1) << Order;
    }

    void DeallocateInternal(const CD3D12BuddyBlockData& Block);

    void DeallocateBlock(uint32_t Offset, uint32_t Order);

    uint32_t GetBuddyOffset(const uint32_t& Offset, const uint32_t& Size)
    {
        return Offset ^ Size;
    }

    uint32_t GetAllocOffsetInBytes(uint32_t Offset) const { return Offset * MinBlockSize; }

private:
    TAllocatorInitData InitData;

    const uint32_t MinBlockSize = 256;

    uint32_t MaxOrder;

    uint32_t TotalAllocSize = 0;

    std::vector<std::set<uint32_t>> FreeBlocks;

    std::vector<CD3D12BuddyBlockData> DeferredDeletionQueue;

    ID3D12Device* D3DDevice;

    CD3D12Resource* BackingResource = nullptr;

    ID3D12Heap* BackingHeap = nullptr;
};

class CD3D12MultiBuddyAllocator
{
public:
    CD3D12MultiBuddyAllocator(ID3D12Device* InDevice, const CD3D12BuddyAllocator::TAllocatorInitData& InInitData);

    ~CD3D12MultiBuddyAllocator();

    bool AllocResource(uint32_t Size, uint32_t Alignment, CD3D12ResourceLocation& ResourceLocation);

    void CleanUpAllocations();

private:
    std::vector<std::shared_ptr<CD3D12BuddyAllocator>> Allocators;

    ID3D12Device* Device;

    CD3D12BuddyAllocator::TAllocatorInitData InitData;
};

class CD3D12UploadBufferAllocator
{
public:
    CD3D12UploadBufferAllocator(ID3D12Device* InDevice);

    void* AllocUploadResource(uint32_t Size, uint32_t Alignment, CD3D12ResourceLocation& ResourceLocation);

    void CleanUpAllocations();

private:
    std::unique_ptr<CD3D12MultiBuddyAllocator> Allocator = nullptr;

    ID3D12Device* D3DDevice = nullptr;
};

class CD3D12DefaultBufferAllocator
{
public:
    CD3D12DefaultBufferAllocator(ID3D12Device* InDevice);

    void AllocDefaultResource(const D3D12_RESOURCE_DESC& ResourceDesc, uint32_t Alignment,
                              CD3D12ResourceLocation& ResourceLocation);

    void CleanUpAllocations();

private:
    std::unique_ptr<CD3D12MultiBuddyAllocator> Allocator = nullptr;

    std::unique_ptr<CD3D12MultiBuddyAllocator> UavAllocator = nullptr;

    ID3D12Device* D3DDevice = nullptr;
};

class CD3D3TextureResourceAllocator
{
public:
    CD3D3TextureResourceAllocator(ID3D12Device* InDevice);

    void AllocTextureResource(const D3D12_RESOURCE_STATES& ResourceState, const D3D12_RESOURCE_DESC& ResourceDesc,
                              CD3D12ResourceLocation& ResourceLocation);

    void CleanUpAllocations();

private:
    std::unique_ptr<CD3D12MultiBuddyAllocator> Allocator = nullptr;

    ID3D12Device* D3DDevice = nullptr;
};
