#include "CameraActor.h"

CCameraActor::CCameraActor(const std::string& Name)
    : CActor(Name)
{
    //CameraComponent
    CameraComponent = AddComponent<CCameraComponent>();

    RootComponent = CameraComponent;
}

CCameraActor::~CCameraActor()
{
}

CCameraComponent* CCameraActor::GetCameraComponent()
{
    return CameraComponent;
}
