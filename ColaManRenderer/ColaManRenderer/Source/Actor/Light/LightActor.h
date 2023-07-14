#pragma once
#include "Actor/Actor.h"
#include "Render/ShadowMap.h"

enum class ELightType
{
    None,
    AmbientLight,
    DirectionalLight,
    PointLight,
    SpotLight,
    AreaLight
};

class CLightActor : public CActor
{
public:
    CLightActor(const std::string& Name, ELightType type);

    ~CLightActor() override;

    ELightType GetType()
    {
        return Type;
    }

    SVector3 GetLightColor()
    {
        return Color;
    }

    virtual void SetLightColor(const SVector3 color)
    {
        Color = color;
    }

    float GetLightIntensity() const
    {
        return Intensity;
    }

    virtual void SetLightIntensity(float intensity)
    {
        Intensity = intensity;
    }

    bool IsCastShadows() const
    {
        return bCastShadows;
    }

    void SetCastShadows(bool bCast)
    {
        bCastShadows = bCast;
    }

    bool IsDrawDebug()
    {
        return bDrawDebug;
    }

    void SetDrawDebug(bool bDraw)
    {
        bDrawDebug = bDraw;
    }

    virtual void UpdateShadowInfo(CShadowMap2D* ShadowMap)
    {
    }

protected:
    ELightType Type = ELightType::None;

    SVector3 Color = SVector3::One;

    float Intensity = 10.0f;

    bool bCastShadows = true;

    bool bDrawDebug = false;

    bool bDrawMesh = false;
};
