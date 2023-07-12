#include "SpotLightActor.h"

CSpotLightActor::CSpotLightActor(const std::string& Name):CLightActor(Name,ELightType::SpotLight)
{
    MeshComponent = AddComponent<CMeshComponent>();

    RootComponent = MeshComponent;

    //Mesh
    MeshComponent->SetMeshName("CylinderMesh");

    //Material
    MeshComponent->SetMaterialInstance("DefaultMatInst");
}

CSpotLightActor::~CSpotLightActor()
{
}

void CSpotLightActor::SetActorTransform(const CTransform& transform)
{
    CLightActor::SetActorTransform(transform);

    SetLightDirection(transform.Rotation);
}

SVector3 CSpotLightActor::GetLightDirection()
{
    return Direction;
}

void CSpotLightActor::UpdateShadowData(CD3D12RHI* d3d12RHI, EShadowMapImpl smImpl)
{
    if(ShadowMap==nullptr)
    {
        const UINT ShadowSize = 1024;
        ShadowMap = std::make_unique<CShadowMap2D>(ShadowSize,ShadowSize,DXGI_FORMAT_R24G8_TYPELESS,d3d12RHI);
        
    }

    SVector3 LightPos = GetActorLocation();
    SVector3 TargetPos = LightPos+GetLightDirection();
    SVector3 LightUp = SVector3::Up;

    float fov = OuterConeAngle*2.0f*(CMath::Pi/180.0f);
    float aspectRation = 1.0f;
    float Near = 0.1f;
    float Far = AttenuationRange;

    ShadowMap->CreatePerspectiveView(LightPos,TargetPos,LightUp,fov,aspectRation,Near,Far);
}

void CSpotLightActor::SetLightDirection(SRotator rotation)
{
    SMatrix r = SMatrix::CreateFromYawPitchRoll(rotation.Yaw*CMath::Pi/180.0f,rotation.Pitch*CMath::Pi/180.0f,rotation.Roll*CMath::Pi/180.0f);
    Direction = r.TransformNormal(SVector3::Up);
}
