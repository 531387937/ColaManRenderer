#include "StaticMeshActor.h"


CStaticMeshActor::CStaticMeshActor(const std::string& Name)
    : CActor(Name)
{
    StaticMeshComponent = AddComponent<CMeshComponent>();

    RootComponent = StaticMeshComponent;
}

CStaticMeshActor::~CStaticMeshActor()
{
}

void CStaticMeshActor::SetMesh(std::string MeshName)
{
    StaticMeshComponent->SetMeshName(MeshName);
}

void CStaticMeshActor::SetMaterialInstance(std::string MaterialInstanceName)
{
    StaticMeshComponent->SetMaterialInstance(MaterialInstanceName);
}

void CStaticMeshActor::SetTextureScale(const SVector2& Scale)
{
    StaticMeshComponent->TexTransform = SMatrix::CreateScale(Scale.x, Scale.y, 1.0f);
}

// void CStaticMeshActor::SetUseSDF(bool bUseSDF)
// {
// 	StaticMeshComponent->bUseSDF = bUseSDF;
// }
