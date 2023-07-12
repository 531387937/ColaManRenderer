#pragma once
#include "D3D12Resource.h"
#include "D3D12View.h"

class CD3D12Texture
{
public:
    CD3D12Resource* GetResource() { return ResourceLocation.UnderlyingResource;}

    ID3D12Resource* GetD3DResource() {return ResourceLocation.UnderlyingResource->D3DResource.Get();}

    void SetRTVClearValue(SVector4 ClearValue) {RTVClearValue = ClearValue;}

    SVector4 GetRTVClearValue() {return RTVClearValue;}

    float* GetRTVClearValuePtr() {return (float*)&RTVClearValue;}

    CD3D12ShaderResourceView* GetSRV(UINT index = 0)
    {
        assert(SRVs.size()>index);

        return SRVs[index].get();
    }

    void AddSRV(std::unique_ptr<CD3D12ShaderResourceView>& SRV)
    {
        SRVs.emplace_back(std::move(SRV));
    }

    CD3D12RenderTargetView* GetRTV(UINT index = 0)
    {
        assert(RTVs.size() > index);

        return RTVs[index].get();
    }

    void AddRTV(std::unique_ptr<CD3D12RenderTargetView>& RTV)
    {
        RTVs.emplace_back(std::move(RTV));
    }

    CD3D12DepthStencilView* GetDSV(UINT index = 0)
    {
        assert(DSVs.size() > index);

        return DSVs[index].get();
    }

    void AddDSV(std::unique_ptr<CD3D12DepthStencilView>& DSV)
    {
        DSVs.emplace_back(std::move(DSV));
    }

    CD3D12UnorderedAccessView* GetUAV(UINT index = 0)
    {
        assert(UAVs.size() > index);

        return UAVs[index].get();
    }

    void AddUAV(std::unique_ptr<CD3D12UnorderedAccessView>& UAV)
    {
        UAVs.emplace_back(std::move(UAV));
    }

public:
    CD3D12ResourceLocation ResourceLocation;

private:

    std::vector<std::unique_ptr<CD3D12ShaderResourceView>> SRVs;

    std::vector<std::unique_ptr<CD3D12RenderTargetView>> RTVs;

    std::vector<std::unique_ptr<CD3D12DepthStencilView>> DSVs;

    std::vector<std::unique_ptr<CD3D12UnorderedAccessView>> UAVs;

private:
    SVector4 RTVClearValue;
};

typedef std::shared_ptr<CD3D12Texture> CD3D12TextureRef;