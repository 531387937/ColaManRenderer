#pragma once
#include <string>

#include "InputLayout.h"
#include "Shader/Shader.h"

struct SGraphicsPSODescriptor
{
    bool operator ==(const SGraphicsPSODescriptor& other) const
    {
        return other.InputLayoutName == InputLayoutName
            && other.Shader == Shader
            && other.PrimitiveTopologyType == PrimitiveTopologyType
            && other.RasterizerDesc.CullMode == RasterizerDesc.CullMode
            && other.DepthStencilDesc.DepthFunc == DepthStencilDesc.DepthFunc;
    }

public:
    std::string InputLayoutName;
    CShader* Shader = nullptr;
    DXGI_FORMAT RTVFormats[8] = {DXGI_FORMAT_R8G8B8A8_UNORM};
    bool _4xMsaaState = false;
    UINT _4xMsaaQuality = 0;
    DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    D3D12_RASTERIZER_DESC RasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    D3D12_BLEND_DESC BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    D3D12_DEPTH_STENCIL_DESC DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    UINT NumRenderTargets = 1;
};

namespace std
{
    template <>
    struct hash<SGraphicsPSODescriptor>
    {
        std::size_t operator()(const SGraphicsPSODescriptor& Descriptor) const
        {
            using std::hash;
            using std::string;

            // Compute individual hash values for each item,
            // and combine them using XOR
            // and bit shifting:
            return (hash<string>()(Descriptor.InputLayoutName)
                ^ (hash<void*>()(Descriptor.Shader) << 1));
        }
    };
}

class CD3D12RHI;

class CGraphicsPSOManager
{
public:
    CGraphicsPSOManager(CD3D12RHI* d3d12RHI, CInputLayoutManager* inputLayoutManager);

    void CreatePSO(const SGraphicsPSODescriptor& descriptor);

    ID3D12PipelineState* GetPSO(const SGraphicsPSODescriptor& descriptor) const;

private:
    CD3D12RHI* D3D12RHI = nullptr;

    CInputLayoutManager* InputLayoutManager = nullptr;

    std::unordered_map<SGraphicsPSODescriptor, Microsoft::WRL::ComPtr<ID3D12PipelineState>> PSOMap;
};

struct SComputePSODescriptor
{
    bool operator==(const SComputePSODescriptor& other) const
    {
        return other.Shader == Shader
            && other.Flags == Flags;
    }

    CShader* Shader = nullptr;
    D3D12_PIPELINE_STATE_FLAGS Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
};

namespace std
{
    template <>
    struct hash<SComputePSODescriptor>
    {
        std::size_t operator()(const SComputePSODescriptor& Descriptor) const
        {
            using std::hash;
            using std::string;

            // Compute individual hash values for each item,
            // and combine them using XOR
            // and bit shifting:
            return (hash<void*>()(Descriptor.Shader)
                ^ (hash<int>()(Descriptor.Flags) << 1));
        }
    };
}

class CComputePSOManager
{
public:
    CComputePSOManager(CD3D12RHI* d3d12RHI);

    bool CreatePSO(const SComputePSODescriptor& descriptor);

    ID3D12PipelineState* GetPSO(const SComputePSODescriptor& descriptor) const;

private:
    CD3D12RHI* d3d12RHI = nullptr;

    std::unordered_map<SComputePSODescriptor, Microsoft::WRL::ComPtr<ID3D12PipelineState>> PSOMap;
};
