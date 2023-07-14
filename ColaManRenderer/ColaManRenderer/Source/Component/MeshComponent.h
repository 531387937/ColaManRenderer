#pragma once
#include <string>

#include "Component.h"
#include "Material/MaterialInstance.h"
#include "Math/Matrix.h"
#include "Mesh/BoundingBox.h"

class CMeshComponent : public CComponent
{
public:
    void SetMeshName(std::string meshName);

    std::string GetMeshName() { return MeshName; }

    bool IsMeshValid() const;

    bool GetLocalBoundingBox(CBoundingBox& box);

    bool GetWorldBoundingBox(CBoundingBox& box);

    void SetMaterialInstance(std::string MaterialInstanceName);

    CMaterialInstance* GetMaterialInstance() { return MaterialInstance; }

public:
    SMatrix TexTransform = SMatrix::Identity;

private:
    std::string MeshName;

    CMaterialInstance* MaterialInstance;
};
