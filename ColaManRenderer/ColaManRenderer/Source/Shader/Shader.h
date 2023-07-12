#pragma once

#include <unordered_map>
#include <wrl/client.h>
#include "D3D12/D3D12Resource.h"
#include "D3D12/D3D12RHI.h"


enum class EShaderType
{
    VERTEX_SHADER,
    PIXEL_SHADER,
    COMPUTE_SHADER,
    HULL_SHADER,
    DOMAIN_SHADER,
    GEOMETRY_SHADER
};

struct SShaderDefines
{
public:
    void GetD3DShaderMacro(std::vector<D3D_SHADER_MACRO>& OutMacros) const;

    bool operator == (const SShaderDefines& other) const;

    void SetDefine(const std::string& name, const std::string& definition);

public:
    std::unordered_map<std::string,std::string> DefinesMap;
};

// declare hash<TShaderDefines>
namespace std
{
    template <>
    struct hash<SShaderDefines>
    {
        std::size_t operator()(const SShaderDefines& Defines) const
        {
            using std::size_t;
            using std::hash;
            using std::string;
            // Compute individual hash values for each string 
            // and combine them using XOR
            // and bit shifting:

            size_t HashValue = 0;
            for (const auto& Pair : Defines.DefinesMap)
            {
                HashValue ^= (hash<string>()(Pair.first) << 1);
                HashValue ^= (hash<string>()(Pair.second) << 1);
            }

            return HashValue;
        }
    };
}

struct SShaderParameter
{
    std::string Name;
    EShaderType ShaderType;
    UINT BindPoint;
    UINT RegisterSpace;
};

struct SShaderCBVParameter:SShaderParameter
{
    CD3D12ConstantBufferRef ConstantBufferRef;
};

struct SShaderSRVParameter:SShaderParameter
{
    UINT BindCount;

    std::vector<CD3D12ShaderResourceView*> SRVList;
};

struct SShaderUAVParameter:SShaderParameter
{
    UINT BindCount;

    std::vector<CD3D12UnorderedAccessView*> UAVList;
};

struct SShaderSamplerParameter:SShaderParameter
{
    
};

struct SShaderInfo
{
    std::string ShaderName;

    std::string FileName;

    SShaderDefines ShaderDefines;

    bool bCreateVS = false;

    std::string VSEntryPoint = "VS";
    
    bool bCreateHS = false;
    
    std::string HSEntryPoint = "HS";

    bool bCreateDS = false;

    std::string DSEntryPoint = "DS";

    bool bCreateGS = false;

    std::string GSEntryPoint = "GS";

    bool bCreatePS = false;

    std::string PSEntryPoint = "PS";

    bool bCreateCS = false;

    std::string CSEntryPoint = "CS";
};

class CShader
{
public:
    CShader(const SShaderInfo& shaderInfo, CD3D12RHI* d3d12RHI);

    void Initialize();

    bool SetParameter(std::string paramName, CD3D12ConstantBufferRef constantBufferRef);

    bool SetParameter(std::string paramName, CD3D12ShaderResourceView* srv);

    bool SetParameter(std::string paramName, const std::vector<CD3D12ShaderResourceView*>& srvList);

    bool SetParameter(std::string paramName, CD3D12UnorderedAccessView* uav);

    bool SetParameter(std::string paramName, const std::vector<CD3D12UnorderedAccessView*>& uavList);

    void BindParameters();

private:
    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines, const std::string& Entrypoint, const std::string& Target);
    
    void GetShaderParameters(Microsoft::WRL::ComPtr<ID3DBlob> passBlob, EShaderType shaderType);

    D3D12_SHADER_VISIBILITY GetShaderVisibility(EShaderType shaderType);

    std::vector<CD3DX12_STATIC_SAMPLER_DESC> CreateStaticSamplers();

    void CreateRootSignature();

    void CheckBindings();

    void ClearBindings();

public:
    SShaderInfo ShaderInfo;

    std::vector<SShaderCBVParameter> CBVParams;

    std::vector<SShaderSRVParameter> SRVParams;

    std::vector<SShaderUAVParameter> UAVParams;

    std::vector<SShaderSamplerParameter>  SamplerParams;

    int CBVSignatureBaseBindSlot = -1;

    int SRVSignatureBindSlot = -1;

    UINT SRVCount = 0;

    int UAVSignatureBindSlot = -1;

    UINT UAVCount = 0;

    int SamplerSignatureBindSlot = -1;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> ShaderPass;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> RootSignature;

private:
    CD3D12RHI* D3D12RHI = nullptr;
};
