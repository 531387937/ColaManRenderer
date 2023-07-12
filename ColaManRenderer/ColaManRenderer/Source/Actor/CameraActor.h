#pragma once

#include "Actor.h"
#include "Component/CameraComponent.h"

class CCameraActor : public CActor
{
public:
	CCameraActor(const std::string& Name);
	~CCameraActor();

	CCameraComponent* GetCameraComponent();

private:
	CCameraComponent* CameraComponent = nullptr;
};
