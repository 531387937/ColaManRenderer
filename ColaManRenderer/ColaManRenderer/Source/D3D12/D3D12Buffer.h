#pragma once
#include "D3D12Resource.h"
#include "D3D12View.h"

class CD3D12Buffer
{
public:
    CD3D12Buffer()
    {
    }

    virtual ~CD3D12Buffer() = default;

    CD3D12Resource* GetResource() { return ResourceLocation.UnderlyingResource; }

public:
    CD3D12ResourceLocation ResourceLocation;
};

class CD3D12ConstantBuffer : public CD3D12Buffer
{
};

using CD3D12ConstantBufferRef = std::shared_ptr<CD3D12ConstantBuffer>;

class CD3D12StructuredBuffer : public CD3D12Buffer
{
public:
    CD3D12ShaderResourceView* GetSRV()
    {
        return SRV.get();
    }

    void SetSRV(std::unique_ptr<CD3D12ShaderResourceView>& srv)
    {
        SRV = std::move(srv);
    }

private:
    std::unique_ptr<CD3D12ShaderResourceView> SRV = nullptr;
};

using CD3D12StructuredBufferRef = std::shared_ptr<CD3D12StructuredBuffer>;

class CD3D12RWStructuredBuffer : public CD3D12Buffer
{
public:
    CD3D12ShaderResourceView* GetSRV()
    {
        return SRV.get();
    }

    void SetSRV(std::unique_ptr<CD3D12ShaderResourceView>& InSRV)
    {
        SRV = std::move(InSRV);
    }

    CD3D12UnorderedAccessView* GetUAV()
    {
        return UAV.get();
    }

    void SetUAV(std::unique_ptr<CD3D12UnorderedAccessView>& InUAV)
    {
        UAV = std::move(InUAV);
    }

private:
    std::unique_ptr<CD3D12ShaderResourceView> SRV = nullptr;

    std::unique_ptr<CD3D12UnorderedAccessView> UAV = nullptr;
};

using CD3D12RWStructuredBufferRef = std::shared_ptr<CD3D12RWStructuredBuffer>;

class CD3D12VertexBuffer : public CD3D12Buffer
{
};

using CD3D12VertexBufferRef = std::shared_ptr<CD3D12VertexBuffer>;


class CD3D12IndexBuffer : public CD3D12Buffer
{
};

using CD3D12IndexBufferRef = std::shared_ptr<CD3D12IndexBuffer>;


class CD3D12ReadBackBuffer : public CD3D12Buffer
{
};

using CD3D12ReadBackBufferRef = std::shared_ptr<CD3D12ReadBackBuffer>;
