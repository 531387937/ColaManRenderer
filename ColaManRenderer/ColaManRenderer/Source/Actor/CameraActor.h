#pragma once

#include "Actor.h"
#include "Component/CameraComponent.h"

class CCameraActor : public CActor
{
public:
    CCameraActor(const std::string& Name);
    ~CCameraActor() override;

    CCameraComponent* GetCameraComponent();

private:
    CCameraComponent* CameraComponent = nullptr;
};
