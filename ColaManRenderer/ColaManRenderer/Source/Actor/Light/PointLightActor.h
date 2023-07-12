#pragma once
#include "LightActor.h"
#include "Component/MeshComponent.h"

class CPointLightActor:public CLightActor
{
public:
    CPointLightActor(const std::string& Name);

    ~CPointLightActor();

    float GetAttenuationRange() const
    {
        return AttenuationRange;
    }

    void SetAttenuationRange(float radius)
    {
        AttenuationRange = radius;
    }

    void UpdateShadowData(CD3D12RHI* d3d12RHI);

    CShadowMapCube* GetShadowMap() {return ShadowMap.get();}
private:
    float AttenuationRange = 10.0f;

    CMeshComponent* MeshComponent = nullptr;

    std::unique_ptr<CShadowMapCube> ShadowMap = nullptr;
};
