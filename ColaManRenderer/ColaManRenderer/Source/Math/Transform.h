#pragma once

#include "Math/Math.h"

struct SRotator
{
    float Roll;
    float Pitch;
    float Yaw;

    SRotator()
        : Roll(0.0f), Pitch(0.0f), Yaw(0.0f)
    {
    }

    SRotator(float InRoll, float InPitch, float InYaw)
        : Roll(InRoll), Pitch(InPitch), Yaw(InYaw)
    {
    }

    static const SRotator Zero;
};

class CTransform
{
public:
    CTransform()
    {
        Location = SVector3::Zero;
        Rotation = SRotator::Zero;
        Scale = SVector3::One;
    }

    SMatrix GetTransformMatrix() const
    {
        SMatrix S = SMatrix::CreateScale(Scale);
        SMatrix R = SMatrix::CreateFromYawPitchRoll(Rotation.Yaw * CMath::Pi / 180.0f,
                                                    Rotation.Pitch * CMath::Pi / 180.0f,
                                                    Rotation.Roll * CMath::Pi / 180.0f);
        SMatrix T = SMatrix::CreateTranslation(Location);

        return S * R * T;
    }

public:
    SVector3 Location;
    SRotator Rotation;
    SVector3 Scale;
};
