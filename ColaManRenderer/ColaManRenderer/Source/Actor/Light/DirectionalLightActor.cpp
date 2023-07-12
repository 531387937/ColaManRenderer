#include "DirectionalLightActor.h"

CDirectionalLightActor::CDirectionalLightActor(const std::string& Name):CLightActor(Name,ELightType::DirectionalLight)
{
    MeshComponent = AddComponent<CMeshComponent>();

    RootComponent = MeshComponent;

    MeshComponent->SetMeshName("CylinderMesh");

    MeshComponent->SetMaterialInstance("DefaultMatInst");
}

CDirectionalLightActor::~CDirectionalLightActor()
{
}

void CDirectionalLightActor::SetActorTransform(const CTransform& transform)
{
    CLightActor::SetActorTransform(transform);

    
}

SVector3 CDirectionalLightActor::GetLightDirection() const
{
    return Direction;
}

void CDirectionalLightActor::UpdateShadowData(CD3D12RHI* d3d12RHI, EShadowMapImpl smImpl)
{
    if(ShadowMap==nullptr)
    {
        const UINT ShadowSize = 1024;
        ShadowMap = std::make_unique<CShadowMap2D>(ShadowSize,ShadowSize,DXGI_FORMAT_R24G8_TYPELESS,d3d12RHI);

        if(smImpl==EShadowMapImpl::VSM)
        {
            
        }
    }
    SVector3 shadowBounds = SVector3(50.0f,50.0f,20.0f);

    SVector3 lightPos = GetActorLocation();
    SVector3 targetPos = lightPos+GetLightDirection();
    SVector3 lightUp = SVector3::Up;

    float left   = - shadowBounds.x * 0.5f;
    float right  =   shadowBounds.x * 0.5f;
    float bottom = - shadowBounds.y * 0.5f;
    float top    =   shadowBounds.y * 0.5f;
    float nearPlane   =   0.0f;
    float farPlane    =   shadowBounds.z;

    ShadowMap->CreateOrthographicView(lightPos, targetPos, lightUp, left, right, bottom, top, nearPlane, farPlane);
}

void CDirectionalLightActor::SetLightDirection(SRotator rotation)
{
    SMatrix R = SMatrix::CreateFromYawPitchRoll(rotation.Yaw*CMath::Pi/180.0f,rotation.Pitch*CMath::Pi/180.0f,rotation.Roll*CMath::Pi/180.0f);
    Direction = R.TransformNormal(SVector3::Up);
}
