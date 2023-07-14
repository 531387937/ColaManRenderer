#include "MeshComponent.h"

#include "Mesh/Mesh.h"
#include "Mesh/MeshRepository.h"

void CMeshComponent::SetMeshName(std::string meshName)
{
    MeshName = meshName;
}

bool CMeshComponent::IsMeshValid() const
{
    return (MeshName != "");
}

bool CMeshComponent::GetLocalBoundingBox(CBoundingBox& box)
{
    //TO DO
    CMesh& mesh = CMeshRepository::Get().MeshMap.at(MeshName);
    CBoundingBox mbox = mesh.GetBoundingBox();

    if (mbox.bInit)
    {
        box = mbox;

        return true;
    }
    return false;
}

bool CMeshComponent::GetWorldBoundingBox(CBoundingBox& box)
{
    CBoundingBox LocalBox;

    if (GetLocalBoundingBox(LocalBox))
    {
        box = LocalBox.Transform(WorldTransform);

        return true;
    }
    return false;
}

void CMeshComponent::SetMaterialInstance(std::string MaterialInstanceName)
{
    //TODO
    //MaterialInstance = TMaterialRepository::Get().GetMaterialInstance(MaterialInstanceName);

    assert(MaterialInstance); //TODO
}
