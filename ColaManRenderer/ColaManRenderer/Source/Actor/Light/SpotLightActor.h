#pragma once
#include "LightActor.h"
#include "Component/MeshComponent.h"

class CSpotLightActor:public CLightActor
{
public:
    CSpotLightActor(const std::string& Name);

    ~CSpotLightActor();

public:
    virtual void SetActorTransform(const CTransform& transform) override;

    SVector3 GetLightDirection();

    float GetAttenuationRange() const
    {
        return AttenuationRange;
    }

    float SetAttenuationRange(float range)
    {
        AttenuationRange = range;
    }

    float GetInnerConeAngle() const
    {
        return InnerConeAngle;
    }

    void SetInnerConeAngle(float angle)
    {
        InnerConeAngle = angle;
    }

    float GetOuterConeAngle() const
    {
        return OuterConeAngle;
    }

    void SetOuterConeAngle(float angle)
    {
        OuterConeAngle = angle;
    }

    float GetBottomRadius() const
    {
        float radians = OuterConeAngle*(CMath::Pi/180.0f);
        float bottomRadius = AttenuationRange*tan(radians);

        return bottomRadius;
    }

    void UpdateShadowData(CD3D12RHI* d3d12RHI,EShadowMapImpl smImpl);

    CShadowMap2D* GetShadowMap() {return ShadowMap.get();}

private:
    void SetLightDirection(SRotator rotation);
private:
    SVector3 Direction = {0.0f,-1.0f,0.0f};

    float AttenuationRange = 10.0f;

    float InnerConeAngle = 0.0f;

    float OuterConeAngle = 40.0f;

    CMeshComponent* MeshComponent = nullptr;

    std::unique_ptr<CShadowMap2D> ShadowMap = nullptr;
    
};
