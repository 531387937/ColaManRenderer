#include "PSO.h"

CGraphicsPSOManager::CGraphicsPSOManager(CD3D12RHI* d3d12RHI, CInputLayoutManager* inputLayoutManager):
    D3D12RHI(d3d12RHI),
    InputLayoutManager(inputLayoutManager)
{
}

void CGraphicsPSOManager::CreatePSO(const SGraphicsPSODescriptor& descriptor)
{
    if (PSOMap.find(descriptor) == PSOMap.end())
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc;
        ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

        // Inputlayout
        std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayout;
        InputLayoutManager->GetInputLayout(descriptor.InputLayoutName, InputLayout);
        PsoDesc.InputLayout = {InputLayout.data(), static_cast<UINT>(InputLayout.size())};

        // Shader
        CShader* Shader = descriptor.Shader;
        auto RootSignature = Shader->RootSignature;
        PsoDesc.pRootSignature = RootSignature.Get();
        PsoDesc.VS =
        {
            reinterpret_cast<BYTE*>(Shader->ShaderPass.at("VS")->GetBufferPointer())
                , Shader->ShaderPass.at("VS")->GetBufferSize()
        };
        PsoDesc.PS = {
            reinterpret_cast<BYTE*>(Shader->ShaderPass.at("PS")->GetBufferPointer())
                , Shader->ShaderPass.at("PS")->GetBufferSize()
        };
        if (Shader->ShaderInfo.bCreateDS)
        {
            PsoDesc.DS = {
                reinterpret_cast<BYTE*>(Shader->ShaderPass.at("DS")->GetBufferPointer())
                    , Shader->ShaderPass.at("DS")->GetBufferSize()
            };
        }
        if (Shader->ShaderInfo.bCreateHS)
        {
            PsoDesc.HS = {
                reinterpret_cast<BYTE*>(Shader->ShaderPass.at("HS")->GetBufferPointer())
                    , Shader->ShaderPass.at("HS")->GetBufferSize()
            };
        }
        if (Shader->ShaderInfo.bCreateGS)
        {
            PsoDesc.GS = {
                reinterpret_cast<BYTE*>(Shader->ShaderPass.at("GS")->GetBufferPointer())
                    , Shader->ShaderPass.at("GS")->GetBufferSize()
            };
        }


        PsoDesc.RasterizerState = descriptor.RasterizerDesc;
        PsoDesc.BlendState = descriptor.BlendDesc;
        PsoDesc.DepthStencilState = descriptor.DepthStencilDesc;
        PsoDesc.SampleMask = UINT_MAX;
        PsoDesc.PrimitiveTopologyType = descriptor.PrimitiveTopologyType;
        PsoDesc.NumRenderTargets = descriptor.NumRenderTargets;
        for (int i = 0; i < 8; i++)
        {
            PsoDesc.RTVFormats[i] = descriptor.RTVFormats[i];
        }
        PsoDesc.SampleDesc.Count = descriptor._4xMsaaState ? 4 : 1;
        PsoDesc.SampleDesc.Quality = descriptor._4xMsaaState ? (descriptor._4xMsaaQuality - 1) : 0;
        PsoDesc.DSVFormat = descriptor.DepthStencilFormat;

        // Create PSO
        Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO;
        auto D3DDevice = D3D12RHI->GetDevice()->GetD3DDevice();
        ThrowIfFailed(D3DDevice->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&PSO)));
        PSOMap.insert({descriptor, PSO});
    }
}

ID3D12PipelineState* CGraphicsPSOManager::GetPSO(const SGraphicsPSODescriptor& descriptor) const
{
    auto iter = PSOMap.find(descriptor);

    if (iter == PSOMap.end())
    {
        assert(0);

        return nullptr;
    }
    return iter->second.Get();
}

CComputePSOManager::CComputePSOManager(CD3D12RHI* d3d12RHI): d3d12RHI(d3d12RHI)
{
}

bool CComputePSOManager::CreatePSO(const SComputePSODescriptor& descriptor)
{
    if (PSOMap.find(descriptor) == PSOMap.end())
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC PsoDesc = {};
        CShader* Shader = descriptor.Shader;
        PsoDesc.pRootSignature = Shader->RootSignature.Get();
        PsoDesc.CS = {
            reinterpret_cast<BYTE*>(Shader->ShaderPass.at("CS")->GetBufferPointer()
                , Shader->ShaderPass.at("CS")->GetBufferSize())
        };
        PsoDesc.Flags = descriptor.Flags;

        Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO;
        auto D3DDevice = d3d12RHI->GetDevice()->GetD3DDevice();
        ThrowIfFailed(D3DDevice->CreateComputePipelineState(&PsoDesc, IID_PPV_ARGS(&PSO)));
        PSOMap.insert({descriptor, PSO});
    }
    return true;
}

ID3D12PipelineState* CComputePSOManager::GetPSO(const SComputePSODescriptor& descriptor) const
{
    auto iter = PSOMap.find(descriptor);

    if (iter == PSOMap.end())
    {
        assert(0); //TODO

        return nullptr;
    }
    return iter->second.Get();
}
