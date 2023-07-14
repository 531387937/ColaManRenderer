#include "Shader.h"
#include "File/FileHelpers.h"
#include "d3d12shader.h"
#include "d3dcompiler.h"
#include "D3D12/D3D12DescriptorCache.h"

void SShaderDefines::GetD3DShaderMacro(std::vector<D3D_SHADER_MACRO>& OutMacros) const
{
    for (const auto& Pair : DefinesMap)
    {
        D3D_SHADER_MACRO Macro;
        Macro.Name = Pair.first.c_str();
        Macro.Definition = Pair.second.c_str();
        OutMacros.push_back(Macro);
    }

    D3D_SHADER_MACRO Macro;
    Macro.Name = nullptr;
    Macro.Definition = nullptr;
    OutMacros.push_back(Macro);

    //return OutMacros;
}

bool SShaderDefines::operator==(const SShaderDefines& other) const
{
    if (DefinesMap.size() != other.DefinesMap.size())
    {
        return false;
    }

    for (const auto& Pair : DefinesMap)
    {
        const std::string Key = Pair.first;
        const std::string Value = Pair.second;

        auto Iter = other.DefinesMap.find(Key);
        if (Iter == other.DefinesMap.end() || Iter->second != Value)
        {
            return false;
        }
    }

    return true;
}

void SShaderDefines::SetDefine(const std::string& name, const std::string& definition)
{
    DefinesMap.insert_or_assign(name, definition);
}

CShader::CShader(const SShaderInfo& shaderInfo, CD3D12RHI* d3d12RHI):ShaderInfo(shaderInfo),D3D12RHI(d3d12RHI)
{
    Initialize();

    assert((ShaderInfo.bCreateVS | ShaderInfo.bCreatePS) ^ ShaderInfo.bCreateCS);
}

void CShader::Initialize()
{
    std::wstring ShaderDir = TFileHelpers::EngineDir() + L"Resource/Shaders/";
    std::wstring FilePath = ShaderDir + CFormatConvert::StrToWStr(ShaderInfo.FileName) + L".hlsl";

    std::vector<D3D_SHADER_MACRO> ShaderMacros;
    ShaderInfo.ShaderDefines.GetD3DShaderMacro(ShaderMacros);

    if (ShaderInfo.bCreateVS)
    {
        auto VSBlob = CompileShader(FilePath, ShaderMacros.data(), ShaderInfo.VSEntryPoint, "vs_5_1");
        ShaderPass["VS"] = VSBlob;

        GetShaderParameters(VSBlob, EShaderType::VERTEX_SHADER);
    }

    if (ShaderInfo.bCreatePS)
    {
        auto PSBlob = CompileShader(FilePath, ShaderMacros.data(), ShaderInfo.PSEntryPoint, "ps_5_1");
        ShaderPass["PS"] = PSBlob;

        GetShaderParameters(PSBlob, EShaderType::PIXEL_SHADER);
    }

    if (ShaderInfo.bCreateHS)
    {
        auto HSBlob = CompileShader(FilePath, ShaderMacros.data(), ShaderInfo.HSEntryPoint, "hs_5_1");
        ShaderPass["HS"] = HSBlob;

        GetShaderParameters(HSBlob, EShaderType::HULL_SHADER);
    }

    if (ShaderInfo.bCreateDS)
    {
        auto DSBlob = CompileShader(FilePath, ShaderMacros.data(), ShaderInfo.DSEntryPoint, "ds_5_1");
        ShaderPass["DS"] = DSBlob;

        GetShaderParameters(DSBlob, EShaderType::DOMAIN_SHADER);
    }

    if (ShaderInfo.bCreateDS)
    {
        auto GSBlob = CompileShader(FilePath, ShaderMacros.data(), ShaderInfo.GSEntryPoint, "gs_5_1");
        ShaderPass["DS"] = GSBlob;

        GetShaderParameters(GSBlob, EShaderType::GEOMETRY_SHADER);
    }

    if (ShaderInfo.bCreateCS)
    {
        auto CSBlob = CompileShader(FilePath, ShaderMacros.data(), ShaderInfo.CSEntryPoint, "cs_5_1");
        ShaderPass["CS"] = CSBlob;

        GetShaderParameters(CSBlob, EShaderType::COMPUTE_SHADER);
    }

    // Create rootSignature
    CreateRootSignature();
}

bool CShader::SetParameter(std::string paramName, CD3D12ConstantBufferRef constantBufferRef)
{
    bool findParam = false;
    for (SShaderCBVParameter& param : CBVParams)
    {
        if (param.Name == paramName)
        {
            param.ConstantBufferRef = constantBufferRef;

            findParam = true;
        }
    }

    return findParam;
}

bool CShader::SetParameter(std::string paramName, CD3D12ShaderResourceView* srv)
{
    std::vector<CD3D12ShaderResourceView*> srvList;
    srvList.push_back(srv);

    return SetParameter(paramName, srvList);
}

bool CShader::SetParameter(std::string paramName, const std::vector<CD3D12ShaderResourceView*>& srvList)
{
    bool findParam = false;

    for (SShaderSRVParameter& param : SRVParams)
    {
        if (param.Name == paramName)
        {
            assert(srvList.size()==param.BindCount);

            param.SRVList = srvList;

            findParam = true;
        }
    }
    return findParam;
}

bool CShader::SetParameter(std::string paramName, CD3D12UnorderedAccessView* uav)
{
    std::vector<CD3D12UnorderedAccessView*> UAVList;
    UAVList.push_back(uav);

    return SetParameter(paramName, UAVList);
}

bool CShader::SetParameter(std::string paramName, const std::vector<CD3D12UnorderedAccessView*>& uavList)
{
    bool FindParam = false;

    for (SShaderUAVParameter& Param : UAVParams)
    {
        if (Param.Name == paramName)
        {
            assert(uavList.size() == Param.BindCount);

            Param.UAVList = uavList;

            FindParam = true;
        }
    }

    return FindParam;
}

void CShader::BindParameters()
{
    auto CommandList = D3D12RHI->GetDevice()->GetCommandList();
    auto DescriptorCache = D3D12RHI->GetDevice()->GetCommandContext()->GetDescriptorCache();

    CheckBindings();

    bool bComputeShader = ShaderInfo.bCreateCS;

    for (int i = 0; i < CBVParams.size(); i++)
    {
        UINT rootParamIdx = CBVSignatureBaseBindSlot + i;
        D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress = CBVParams[i].ConstantBufferRef->ResourceLocation.
                                                                   GPUVirtualAddress;

        if (bComputeShader)
        {
            CommandList->SetComputeRootConstantBufferView(rootParamIdx, GPUVirtualAddress);
        }
        else
        {
            CommandList->SetGraphicsRootConstantBufferView(rootParamIdx, GPUVirtualAddress);
        }
    }

    if (SRVCount > 0)
    {
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> srcDescriptors;
        srcDescriptors.resize(SRVCount);

        for (const SShaderSRVParameter& param : SRVParams)
        {
            for (UINT i = 0; i < param.SRVList.size(); i++)
            {
                UINT index = param.BindPoint + i;
                srcDescriptors[index] = param.SRVList[i]->GetDescriptorHandle();
            }
        }

        UINT rootParamIdx = SRVSignatureBindSlot;
        auto gpuDescriptorHandle = DescriptorCache->AppendCbvSrvUavDescriptors(srcDescriptors);

        if (bComputeShader)
        {
            CommandList->SetComputeRootDescriptorTable(rootParamIdx, gpuDescriptorHandle);
        }
        else
        {
            CommandList->SetGraphicsRootDescriptorTable(rootParamIdx, gpuDescriptorHandle);
        }
    }

    if (UAVCount > 0)
    {
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> SrcDescriptors;
        SrcDescriptors.resize(UAVCount);

        for (const SShaderUAVParameter& Param : UAVParams)
        {
            for (UINT i = 0; i < Param.UAVList.size(); i++)
            {
                UINT Index = Param.BindPoint + i;
                SrcDescriptors[Index] = Param.UAVList[i]->GetDescriptorHandle();
            }
        }

        UINT RootParamIdx = UAVSignatureBindSlot;
        auto GpuDescriptorHandle = DescriptorCache->AppendCbvSrvUavDescriptors(SrcDescriptors);

        if (bComputeShader)
        {
            CommandList->SetComputeRootDescriptorTable(RootParamIdx, GpuDescriptorHandle);
        }
        else
        {
            assert(0);
        }
    }

    ClearBindings();
}

Microsoft::WRL::ComPtr<ID3DBlob> CShader::CompileShader(const std::wstring& Filename, const D3D_SHADER_MACRO* Defines,
                                                        const std::string& Entrypoint, const std::string& Target)
{
    UINT CompileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = S_OK;

    Microsoft::WRL::ComPtr<ID3DBlob> ByteCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> Errors;
    hr = D3DCompileFromFile(Filename.c_str(), Defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                            Entrypoint.c_str(), Target.c_str(), CompileFlags, 0, &ByteCode, &Errors);

    if (Errors != nullptr)
        OutputDebugStringA(static_cast<char*>(Errors->GetBufferPointer()));

    ThrowIfFailed(hr);

    return ByteCode;
}

D3D12_SHADER_VISIBILITY CShader::GetShaderVisibility(EShaderType shaderType)
{
    D3D12_SHADER_VISIBILITY shaderVisibility;
    if (shaderType == EShaderType::VERTEX_SHADER)
    {
        shaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    }
    else if (shaderType == EShaderType::PIXEL_SHADER)
    {
        shaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    }
    else if (shaderType == EShaderType::COMPUTE_SHADER)
    {
        shaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    }
    else if (shaderType == EShaderType::HULL_SHADER)
    {
        shaderVisibility = D3D12_SHADER_VISIBILITY_HULL;
    }
    else if (shaderType == EShaderType::DOMAIN_SHADER)
    {
        shaderVisibility = D3D12_SHADER_VISIBILITY_DOMAIN;
    }
    else if (shaderType == EShaderType::GEOMETRY_SHADER)
    {
        shaderVisibility = D3D12_SHADER_VISIBILITY_GEOMETRY;
    }
    else
    {
        assert(0);
    }

    return shaderVisibility;
}

std::vector<CD3DX12_STATIC_SAMPLER_DESC> CShader::CreateStaticSamplers()
{
    CD3DX12_STATIC_SAMPLER_DESC PointWrap(
        0, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    CD3DX12_STATIC_SAMPLER_DESC PointClamp(
        1, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

    CD3DX12_STATIC_SAMPLER_DESC LinearWrap(
        2, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    CD3DX12_STATIC_SAMPLER_DESC LinearClamp(
        3, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

    CD3DX12_STATIC_SAMPLER_DESC AnisotropicWrap(
        4, // shaderRegister
        D3D12_FILTER_ANISOTROPIC, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressW
        0.0f, // mipLODBias
        8); // maxAnisotropy

    CD3DX12_STATIC_SAMPLER_DESC AnisotropicClamp(
        5, // shaderRegister
        D3D12_FILTER_ANISOTROPIC, // filter
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP, // addressW
        0.0f, // mipLODBias
        8); // maxAnisotropy

    CD3DX12_STATIC_SAMPLER_DESC Shadow(
        6, // shaderRegister
        D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, // addressW
        0.0f, // mipLODBias
        16, // maxAnisotropy
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

    CD3DX12_STATIC_SAMPLER_DESC DepthMap(
        7, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, // addressU
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, // addressV
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, // addressW
        0.0f,
        0,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);


    std::vector<CD3DX12_STATIC_SAMPLER_DESC> StaticSamplers;
    StaticSamplers.push_back(PointWrap);
    StaticSamplers.push_back(PointClamp);
    StaticSamplers.push_back(LinearWrap);
    StaticSamplers.push_back(LinearClamp);
    StaticSamplers.push_back(AnisotropicWrap);
    StaticSamplers.push_back(AnisotropicClamp);
    StaticSamplers.push_back(Shadow);
    StaticSamplers.push_back(DepthMap);

    return StaticSamplers;
}

void CShader::CreateRootSignature()
{
    std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameter;

    for (const SShaderCBVParameter& param : CBVParams)
    {
        if (CBVSignatureBaseBindSlot == -1)
        {
            CBVSignatureBaseBindSlot = static_cast<UINT>(slotRootParameter.size());
        }

        CD3DX12_ROOT_PARAMETER rootParam;

        rootParam.InitAsConstantBufferView(param.BindPoint, param.RegisterSpace, GetShaderVisibility(param.ShaderType));
        slotRootParameter.push_back(rootParam);
    }

    for (const SShaderSRVParameter& Param : SRVParams)
    {
        SRVCount += Param.BindCount;
    }

    if (SRVCount > 0)
    {
        SRVSignatureBindSlot = static_cast<UINT>(slotRootParameter.size());

        CD3DX12_DESCRIPTOR_RANGE SRVTable;
        SRVTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRVCount, 0, 0);

        CD3DX12_ROOT_PARAMETER RootParam;
        D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //TODO
        RootParam.InitAsDescriptorTable(1, &SRVTable, ShaderVisibility);
        slotRootParameter.push_back(RootParam);
    }

    for (const SShaderUAVParameter& Param : UAVParams)
    {
        UAVCount += Param.BindCount;
    }

    if (UAVCount > 0)
    {
        UAVSignatureBindSlot = static_cast<UINT>(slotRootParameter.size());

        CD3DX12_DESCRIPTOR_RANGE UAVTable;
        UAVTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UAVCount, 0, 0);

        CD3DX12_ROOT_PARAMETER RootParam;
        D3D12_SHADER_VISIBILITY ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //TODO
        RootParam.InitAsDescriptorTable(1, &UAVTable, ShaderVisibility);
        slotRootParameter.push_back(RootParam);
    }
    auto StaticSamplers = CreateStaticSamplers();

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(static_cast<UINT>(slotRootParameter.size()), slotRootParameter.data(),
                                            static_cast<UINT>(StaticSamplers.size()), StaticSamplers.data(),
                                            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                             serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
    }
    ThrowIfFailed(hr);

    ThrowIfFailed(D3D12RHI->GetDevice()->GetD3DDevice()->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&RootSignature)));
}


void CShader::CheckBindings()
{
    for (SShaderCBVParameter& Param : CBVParams)
    {
        assert(Param.ConstantBufferRef);
    }

    for (SShaderSRVParameter& Param : SRVParams)
    {
        assert(Param.SRVList.size() > 0);
    }

    for (SShaderUAVParameter& Param : UAVParams)
    {
        assert(Param.UAVList.size() > 0);
    }
}

void CShader::ClearBindings()
{
    for (SShaderCBVParameter& Param : CBVParams)
    {
        Param.ConstantBufferRef = nullptr;
    }

    for (SShaderSRVParameter& Param : SRVParams)
    {
        Param.SRVList.clear();
    }

    for (SShaderUAVParameter& Param : UAVParams)
    {
        Param.UAVList.clear();
    }
}

void CShader::GetShaderParameters(Microsoft::WRL::ComPtr<ID3DBlob> passBlob, EShaderType shaderType)
{
    ID3D12ShaderReflection* reflection = nullptr;
    D3DReflect(passBlob->GetBufferPointer(), passBlob->GetBufferSize(), IID_ID3D12ShaderReflection,
               (void**)&reflection);

    D3D12_SHADER_DESC shaderDesc;
    reflection->GetDesc(&shaderDesc);

    for (UINT i = 0; i < shaderDesc.BoundResources; i++)
    {
        D3D12_SHADER_INPUT_BIND_DESC resourceDesc;
        reflection->GetResourceBindingDesc(i, &resourceDesc);

        auto shaderVarName = resourceDesc.Name;
        auto resourceType = resourceDesc.Type;
        auto registerSpace = resourceDesc.Space;
        auto bindPoint = resourceDesc.BindPoint;
        auto bindCount = resourceDesc.BindCount;

        if (resourceType == D3D_SIT_CBUFFER)
        {
            SShaderCBVParameter param;
            param.Name = shaderVarName;
            param.ShaderType = shaderType;
            param.BindPoint = bindPoint;
            param.RegisterSpace = registerSpace;

            CBVParams.push_back(param);
        }
        else if (resourceType == D3D_SIT_STRUCTURED || resourceType == D3D_SIT_TEXTURE)
        {
            SShaderSRVParameter param;
            param.Name = shaderVarName;
            param.ShaderType = shaderType;
            param.BindPoint = bindPoint;
            param.RegisterSpace = registerSpace;
            param.BindCount = bindCount;

            SRVParams.push_back(param);
        }
        else if (resourceType == D3D_SIT_UAV_RWSTRUCTURED || resourceType == D3D_SIT_UAV_RWTYPED)
        {
            assert(shaderType == EShaderType::COMPUTE_SHADER);

            SShaderUAVParameter param;
            param.Name = shaderVarName;
            param.ShaderType = shaderType;
            param.BindPoint = bindPoint;
            param.RegisterSpace = registerSpace;
            param.BindCount = bindCount;

            UAVParams.push_back(param);
        }
        else if (resourceType == D3D_SIT_SAMPLER)
        {
            SShaderSamplerParameter param;
            param.Name = shaderVarName;
            param.ShaderType = shaderType;
            param.BindPoint = bindPoint;
            param.RegisterSpace = registerSpace;

            SamplerParams.push_back(param);
        }
    }
}
