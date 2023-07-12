#include "MaterialInstance.h"

struct SMaterialConstants;

CMaterialInstance::CMaterialInstance(CMaterial* parent, const std::string& name):Material(parent),Name(name)
{
    Parameters = Material->Parameters;
}

void CMaterialInstance::SetTextureParameter(const std::string& parameter, const std::string& textureName)
{
    auto iter = Parameters.TextureMap.find(parameter);

    if(iter!=Parameters.TextureMap.end())
    {
        iter->second = textureName;
    }
}

void CMaterialInstance::CreateMaterialConstantBuffer(CD3D12RHI* d3d12RHI)
{
    //TO DO
    SMaterialConstants MatConst;

    MatConst.DiffuseAlbedo = Parameters.DiffuseAlbedo;
    MatConst.FresnelR0 =Parameters.FresnelR0;
    MatConst.Roughness = Parameters.Roughness;
    MatConst.MatTransform = Parameters.MatTransform;
    MatConst.EmissiveColor = Parameters.EmissiveColor;
    MatConst.ShadingModel = (UINT)Material->ShadingModel;

    //Create ConstantBuffer
    MaterialConstantBuffer = d3d12RHI->CreateConstantBuffer(&MatConst, sizeof(MatConst));
}
