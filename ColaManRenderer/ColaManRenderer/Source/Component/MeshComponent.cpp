#include "MeshComponent.h"

#include "Mesh/Mesh.h"

void CMeshComponent::SetMeshName(std::string meshName)
{
    MeshName = meshName;
}

bool CMeshComponent::IsMeshValid() const
{
    return (MeshName!="");
}

bool CMeshComponent::GetLocalBoundingBox(CBoundingBox& box)
{
    //TO DO
    //CMesh& mesh = 
}

bool CMeshComponent::GetWorldBoundingBox(CBoundingBox& box)
{
    CBoundingBox LocalBox;
	
    if (GetLocalBoundingBox(LocalBox))
    {
        box = LocalBox.Transform(WorldTransform);

        return true;
    }
    else
    {
        return false;
    }
}

void CMeshComponent::SetMaterialInstance(std::string MaterialInstanceName)
{
    //TODO
    //MaterialInstance = TMaterialRepository::Get().GetMaterialInstance(MaterialInstanceName);

    assert(MaterialInstance);  //TODO
}
