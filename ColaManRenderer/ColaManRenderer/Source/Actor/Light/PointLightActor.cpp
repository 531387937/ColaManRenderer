#include "PointLightActor.h"

CPointLightActor::CPointLightActor(const std::string& Name): CLightActor(Name, ELightType::PointLight)
{
    MeshComponent = AddComponent<CMeshComponent>();

    RootComponent = MeshComponent;

    MeshComponent->SetMeshName("SphereMesh");

    MeshComponent->SetMaterialInstance("DefaultMatInst");
}

CPointLightActor::~CPointLightActor()
{
}

void CPointLightActor::UpdateShadowData(CD3D12RHI* d3d12RHI)
{
    if (ShadowMap == nullptr)
    {
        const UINT ShadowSize = 1024;
        ShadowMap = std::make_unique<CShadowMapCube>(ShadowSize, DXGI_FORMAT_R24G8_TYPELESS, d3d12RHI);
    }

    SVector3 LightPos = GetActorLocation();

    float Near = 0.1f;

    float Far = AttenuationRange;

    ShadowMap->CreatePerspectiveViews(LightPos, Near, Far);
}
