#include "Material.h"

CMaterial::CMaterial(const std::string& name, const std::string& shaderName):Name(name),ShaderName(shaderName)
{
}

CShader* CMaterial::GetShader(const SShaderDefines& shaderDefines, CD3D12RHI* d3d12RHI)
{
    auto Iter = ShaderMap.find(shaderDefines);

    if (Iter == ShaderMap.end())
    {
        // Create new shader
        SShaderInfo ShaderInfo;
        ShaderInfo.ShaderName = ShaderName;
        ShaderInfo.FileName = ShaderName;
        ShaderInfo.ShaderDefines = shaderDefines;
        ShaderInfo.bCreateVS = true;
        ShaderInfo.bCreatePS = true;
        std::unique_ptr<CShader> NewShader = std::make_unique<CShader>(ShaderInfo, d3d12RHI);

        ShaderMap.insert({ shaderDefines, std::move(NewShader)});

        return ShaderMap[shaderDefines].get();
    }
    else
    {
        return Iter->second.get();
    }
}
