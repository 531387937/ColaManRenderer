#pragma once
#include "Material.h"

class CMaterialInstance
{
public:
    CMaterialInstance(CMaterial* parent, const std::string& name);

    void SetTextureParameter(const std::string& parameter, const std::string& textureName);

    void CreateMaterialConstantBuffer(CD3D12RHI* d3d12RHI);

public:
    CMaterial* Material = nullptr;

    std::string Name;

    SMaterialParameters Parameters;

    CD3D12ConstantBufferRef MaterialConstantBuffer = nullptr;
};
