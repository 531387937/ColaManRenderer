#pragma once
#include "Math/Math.h"

class CRay
{
public:
    CRay();

    CRay(const SVector3& origin, SVector3 direction, float matDist = CMath::Infinity);

public:
    SVector3 Origin;

    SVector3 Direction;

    mutable float MaxDist;
};
