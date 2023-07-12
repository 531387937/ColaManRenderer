#pragma once
#include <string>
#include <unordered_map>
#include "D3D12/D3D12Utils.h"
#include "Shader/Shader.h"
#include "Math/Math.h"

enum class EShadingMode
{
    DefaultLit,
    Unlit,
};

struct SMaterialParameters
{
public:
    SVector4 DiffuseAlbedo = {1.0f,1.0f,1.0f,1.0f};

    SVector3 FresnelR0 = {0.01f,0.01f,0.01f};

    float Roughness = 0.25f;

    SVector3 EmissiveColor = {0.0f,0.0f,0.0f};

    SMatrix MatTransform = SMatrix::Identity;

    std::unordered_map<std::string,std::string> TextureMap;
};

struct SMaterialRenderState
{
    D3D12_CULL_MODE CullMode = D3D12_CULL_MODE_BACK;

    D3D12_COMPARISON_FUNC DepthFunc = D3D12_COMPARISON_FUNC_LESS;
};



class CMaterial
{
public:
    CMaterial(const std::string& name, const std::string& shaderName);

    CShader* GetShader(const SShaderDefines& shaderDefines,CD3D12RHI* d3d12RHI);

public:
    std::string Name;

    EShadingMode ShadingModel = EShadingMode::DefaultLit;

    SMaterialParameters Parameters;

    SMaterialParameters RenderState;

private:
    std::string ShaderName;

    std::unordered_map<SShaderDefines,std::unique_ptr<CShader>> ShaderMap;
};
