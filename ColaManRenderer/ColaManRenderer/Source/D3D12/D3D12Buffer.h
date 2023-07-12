#pragma once
#include "D3D12Resource.h"
#include "D3D12View.h"

class CD3D12Buffer
{
public:
    CD3D12Buffer(){}

    virtual ~CD3D12Buffer();

    CD3D12Resource* GetResource(){return ResourceLocation.UnderlyingResource;}

public:
    CD3D12ResourceLocation ResourceLocation;
};

class CD3D12ConstantBuffer:public CD3D12Buffer
{
    
};

typedef std::shared_ptr<CD3D12ConstantBuffer> CD3D12ConstantBufferRef;

class CD3D12StructuredBuffer:public CD3D12Buffer
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
typedef std::shared_ptr<CD3D12StructuredBuffer> CD3D12StructuredBufferRef;

class CD3D12RWStructuredBuffer: public CD3D12Buffer
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
typedef std::shared_ptr<CD3D12RWStructuredBuffer> CD3D12RWStructuredBufferRef;

class CD3D12VertexBuffer : public CD3D12Buffer
{

};
typedef std::shared_ptr<CD3D12VertexBuffer> CD3D12VertexBufferRef;


class CD3D12IndexBuffer : public CD3D12Buffer
{

};
typedef std::shared_ptr<CD3D12IndexBuffer> CD3D12IndexBufferRef;


class CD3D12ReadBackBuffer : public CD3D12Buffer
{

};
typedef std::shared_ptr<CD3D12ReadBackBuffer> CD3D12ReadBackBufferRef;