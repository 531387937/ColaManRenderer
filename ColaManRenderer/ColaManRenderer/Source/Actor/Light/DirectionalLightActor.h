#pragma once
#include "LightActor.h"
#include "Component/MeshComponent.h"
#include "Math/Vector3.h"
#include "Render/ShadowMap.h"

class CDirectionalLightActor : public CLightActor
{
public:
    CDirectionalLightActor(const std::string& Name);

    ~CDirectionalLightActor() override;

    void SetActorTransform(const CTransform& transform) override;

    SVector3 GetLightDirection() const;

    void UpdateShadowData(CD3D12RHI* d3d12RHI, EShadowMapImpl smImpl);

    CShadowMap2D* GetShadowMap() { return ShadowMap.get(); }

    //CD3D12TextureRef GetV
private:
    void SetLightDirection(SRotator rotation);

private:
    SVector3 Direction;

    CMeshComponent* MeshComponent = nullptr;

    std::unique_ptr<CShadowMap2D> ShadowMap = nullptr;

    //CD3D12TextureRef VSMTexture = nullptr;
};
