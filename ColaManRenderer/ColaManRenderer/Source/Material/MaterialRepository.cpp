#include "MaterialRepository.h"

CMaterialRepository& CMaterialRepository::Get()
{
    static CMaterialRepository Instance;
    return Instance;
}

void CMaterialRepository::Load()
{
    //--------------------------------------DefaultMat------------------------------------------------------
    {
        // Material
        CMaterial* DefaultMat = CreateMaterial("DefaultMat", "BasePassDefault");

        SMaterialParameters& Parameters = DefaultMat->Parameters;
        Parameters.TextureMap.emplace("BaseColorTexture", "NullTex");
        Parameters.TextureMap.emplace("NormalTexture", "NullTex");
        Parameters.TextureMap.emplace("MetallicTexture", "NullTex");
        Parameters.TextureMap.emplace("RoughnessTexture", "NullTex");

        // MaterialInstances
        CreateDefaultMaterialInstance(DefaultMat);

        {
            CMaterialInstance* AssaultRifleMatInst = CreateMaterialInstance(DefaultMat, "AssaultRifleMatInst");

            AssaultRifleMatInst->SetTextureParameter("BaseColorTexture", "AssaultRifle_BaseColor");
            AssaultRifleMatInst->SetTextureParameter("NormalTexture", "AssaultRifle_Normal");
            AssaultRifleMatInst->SetTextureParameter("MetallicTexture", "AssaultRifle_Metallic");
            AssaultRifleMatInst->SetTextureParameter("RoughnessTexture", "AssaultRifle_Roughness");
        }

        {
            CMaterialInstance* CyborgWeaponMatInst = CreateMaterialInstance(DefaultMat, "CyborgWeaponMatInst");

            CyborgWeaponMatInst->SetTextureParameter("BaseColorTexture", "CyborgWeapon_BaseColor");
            CyborgWeaponMatInst->SetTextureParameter("NormalTexture", "CyborgWeapon_Normal");
            CyborgWeaponMatInst->SetTextureParameter("MetallicTexture", "CyborgWeapon_Metallic");
            CyborgWeaponMatInst->SetTextureParameter("RoughnessTexture", "CyborgWeapon_Roughness");
        }

        {
            CMaterialInstance* HelmetMatInst = CreateMaterialInstance(DefaultMat, "HelmetMatInst");
            HelmetMatInst->SetTextureParameter("BaseColorTexture", "Helmet_BaseColor");
            HelmetMatInst->SetTextureParameter("NormalTexture", "Helmet_Normal");
            HelmetMatInst->SetTextureParameter("MetallicTexture", "Helmet_Metallic");
            HelmetMatInst->SetTextureParameter("RoughnessTexture", "Helmet_Roughness");
        }

        {
            CMaterialInstance* RustedIronMatInst = CreateMaterialInstance(DefaultMat, "RustedIronMatInst");
            RustedIronMatInst->SetTextureParameter("BaseColorTexture", "RustedIron_BaseColor");
            RustedIronMatInst->SetTextureParameter("NormalTexture", "RustedIron_Normal");
            RustedIronMatInst->SetTextureParameter("MetallicTexture", "RustedIron_Metallic");
            RustedIronMatInst->SetTextureParameter("RoughnessTexture", "RustedIron_Roughness");
        }

        {
            CMaterialInstance* GrayGraniteMatInst = CreateMaterialInstance(DefaultMat, "GrayGraniteMatInst");
            GrayGraniteMatInst->SetTextureParameter("BaseColorTexture", "GrayGranite_BaseColor");
            GrayGraniteMatInst->SetTextureParameter("NormalTexture", "GrayGranite_Normal");
            GrayGraniteMatInst->SetTextureParameter("MetallicTexture", "GrayGranite_Metallic");
            GrayGraniteMatInst->SetTextureParameter("RoughnessTexture", "GrayGranite_Roughness");
        }

        {
            CMaterialInstance* FloorMatInst = CreateMaterialInstance(DefaultMat, "FloorMatInst");
            FloorMatInst->SetTextureParameter("BaseColorTexture", "Floor_BaseColor");
            FloorMatInst->SetTextureParameter("NormalTexture", "Floor_Normal");
            FloorMatInst->SetTextureParameter("MetallicTexture", "Floor_Metallic");
            FloorMatInst->SetTextureParameter("RoughnessTexture", "Floor_Roughness");
        }

        {
            CMaterialInstance* ColumnMatInst = CreateMaterialInstance(DefaultMat, "ColumnMatInst");
            ColumnMatInst->SetTextureParameter("BaseColorTexture", "Column_BaseColor");
            ColumnMatInst->SetTextureParameter("NormalTexture", "Column_Normal");
            ColumnMatInst->SetTextureParameter("MetallicTexture", "NullTex");
            ColumnMatInst->SetTextureParameter("RoughnessTexture", "Column_Roughness");
        }

        {
            CMaterialInstance* EmissiveMatInst = CreateMaterialInstance(DefaultMat, "EmissiveMatInst");

            SMaterialParameters& Parameters = EmissiveMatInst->Parameters;
            Parameters.EmissiveColor = SVector3(1.0f);
            EmissiveMatInst->SetTextureParameter("BaseColorTexture", "NullTex");
            EmissiveMatInst->SetTextureParameter("NormalTexture", "NullTex");
            EmissiveMatInst->SetTextureParameter("MetallicTexture", "NullTex");
            EmissiveMatInst->SetTextureParameter("RoughnessTexture", "NullTex");
        }
    }


    //--------------------------------------SkyMat------------------------------------------------------
    {
        // Material
        CMaterial* SkyMat = CreateMaterial("SkyMat", "BasePassSky");

        SMaterialParameters& Parameters = SkyMat->Parameters;
        Parameters.TextureMap.emplace("SkyCubeTexture", "Shiodome_Stairs");

        SMaterialRenderState& RenderState = SkyMat->RenderState;
        RenderState.CullMode = D3D12_CULL_MODE_NONE;
        RenderState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

        SkyMat->ShadingModel = EShadingMode::Unlit;

        // MaterialInstance
        CreateDefaultMaterialInstance(SkyMat);
    }
}

void CMaterialRepository::Unload()
{
    MaterialInstanceMap.clear();

    MaterialMap.clear();
}

CMaterial* CMaterialRepository::CreateMaterial(const std::string& MaterialName, const std::string& ShaderName)
{
    MaterialMap.insert({MaterialName, std::make_unique<CMaterial>(MaterialName, ShaderName)});

    return MaterialMap[MaterialName].get();
}

CMaterialInstance* CMaterialRepository::CreateMaterialInstance(CMaterial* Material,
                                                               const std::string& MaterialInstanceName)
{
    MaterialInstanceMap.insert({
        MaterialInstanceName, std::make_unique<CMaterialInstance>(Material, MaterialInstanceName)
    });

    return MaterialInstanceMap[MaterialInstanceName].get();
}

void CMaterialRepository::CreateDefaultMaterialInstance(CMaterial* Material)
{
    CreateMaterialInstance(Material, Material->Name + "Inst");
}

CMaterialInstance* CMaterialRepository::GetMaterialInstance(const std::string& MaterialInstanceName) const
{
    CMaterialInstance* Result = nullptr;

    auto Iter = MaterialInstanceMap.find(MaterialInstanceName);
    if (Iter != MaterialInstanceMap.end())
    {
        Result = Iter->second.get();
    }

    return Result;
}
