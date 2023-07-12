#pragma once
#include <unordered_map>

#include "D3D12/D3D12Buffer.h"
#include "Material/Material.h"

class CMeshComponent;

struct SMeshBatch
{
    std::string MeshName;

    std::string InputLayoutName;

    CD3D12ConstantBufferRef ObjConstantBuffer = nullptr;

    CMeshComponent* MeshComponent = nullptr;

    //bool bUseSDF = false;
};

struct SMeshCommand
{
    struct SMeshShaderParameters
    {
        std::unordered_map<std::string, CD3D12ConstantBufferRef> CBVParams;

        std::unordered_map<std::string,std::vector<CD3D12ShaderResourceView*>> SRVParams;
    };

public:
    void SetShaderParameter(std::string param,CD3D12ConstantBufferRef CBV)
    {
        ShaderParameters.CBVParams.insert(std::make_pair(param,CBV));
    }

    void SetShaderParameter(std::string param, CD3D12ShaderResourceView* SRV)
    {
        std::vector<CD3D12ShaderResourceView*> SRVs;
        SRVs.push_back(SRV);

        ShaderParameters.SRVParams.insert(std::make_pair(param,SRVs));
    }

    void ApplyShaderParamters(CShader* Shader) const
    {
        if(Shader)
        {
            for(const auto& item:ShaderParameters.CBVParams)
            {
                Shader->SetParameter(item.first,item.second);
            }

            for(const auto& item:ShaderParameters.SRVParams)
            {
                Shader->SetParameter(item.first,item.second);
            }
        }
    }

public:
    std::string MeshName;

    SMaterialRenderState RenderState;

    SMeshShaderParameters ShaderParameters;
};

typedef std::vector<SMeshCommand> CMeshCommandList;