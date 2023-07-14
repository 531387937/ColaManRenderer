#pragma once

#include <unordered_map>
#include <string>
#include "Material.h"
#include "MaterialInstance.h"

class CMaterialRepository
{
public:
    static CMaterialRepository& Get();

    void Load();

    void Unload();

    CMaterialInstance* GetMaterialInstance(const std::string& MaterialInstanceName) const;

private:
    CMaterial* CreateMaterial(const std::string& MaterialName, const std::string& ShaderName);

    CMaterialInstance* CreateMaterialInstance(CMaterial* Material, const std::string& MaterialInstanceName);

    void CreateDefaultMaterialInstance(CMaterial* Material);

public:
    std::unordered_map<std::string /*MaterialName*/, std::unique_ptr<CMaterial>> MaterialMap;

    std::unordered_map<std::string /*MaterialInstanceName*/, std::unique_ptr<CMaterialInstance>> MaterialInstanceMap;
};
