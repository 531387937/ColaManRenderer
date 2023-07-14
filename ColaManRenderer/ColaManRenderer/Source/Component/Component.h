#pragma once
#include "Math/Transform.h"

class CComponent
{
public:
    CComponent()
    {
    }

    virtual ~CComponent()
    {
    }

public:
    virtual void SetWorldLocation(const SVector3& Location)
    {
        WorldTransform.Location = Location;
    }

    SVector3 GetWorldLocation() const
    {
        return WorldTransform.Location;
    }

    virtual void SetWorldRotation(const SRotator& Rotation)
    {
        WorldTransform.Rotation = Rotation;
    }

    SRotator GetWorldRotation() const
    {
        return WorldTransform.Rotation;
    }

    void SetWorldTransform(const CTransform& Transform)
    {
        WorldTransform = Transform;
    }

    CTransform GetWorldTransform() const
    {
        return WorldTransform;
    }

    void SetPrevWorldTransform(const CTransform& Transform)
    {
        PrevWorldTransform = Transform;
    }

    CTransform GetPrevWorldTransform() const
    {
        return PrevWorldTransform;
    }

protected:
    CTransform RelativeTransform; //TODO

    CTransform WorldTransform;

    CTransform PrevWorldTransform;
};
