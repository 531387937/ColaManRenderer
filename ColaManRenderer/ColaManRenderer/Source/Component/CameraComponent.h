#pragma once

#include "Component.h"

class CCameraComponent : public CComponent
{
public:
    CCameraComponent();
    ~CCameraComponent() override;

    // Location
    void SetWorldLocation(const SVector3& Location) override;

    // Get frustum properties.
    float GetNearZ() const;
    float GetFarZ() const;
    float GetAspect() const;
    float GetFovY() const;
    float GetFovX() const;

    // Get near and far plane dimensions in view space coordinates.
    float GetNearWindowWidth() const;
    float GetNearWindowHeight() const;
    float GetFarWindowWidth() const;
    float GetFarWindowHeight() const;

    // Set frustum.
    void SetLens(float fovY, float aspect, float zn, float zf);

    // Define camera space via LookAt parameters.
    void LookAt(const SVector3& pos, const SVector3& target, const SVector3& up);

    // Get View/Proj matrices.
    SMatrix GetView() const;
    SMatrix GetProj() const;

    // Move the camera a distance.
    void MoveRight(float Dist);
    void MoveForward(float Dist);
    void MoveUp(float Dist);

    // Rotate the camera.
    void Pitch(float Degrees);
    void RotateY(float Degrees);

    // After modifying camera position/orientation, call to rebuild the view matrix.
    void UpdateViewMatrix();

    void SetPrevViewProj(const SMatrix& VP);

    SMatrix GetPrevViewProj() const;

private:
    // Camera coordinate system with coordinates relative to world space.
    SVector3 Right = {1.0f, 0.0f, 0.0f};
    SVector3 Up = {0.0f, 1.0f, 0.0f};
    SVector3 Look = {0.0f, 0.0f, 1.0f};

    // Cache frustum properties.
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float Aspect = 0.0f;
    float FovY = 0.0f;
    float NearWindowHeight = 0.0f;
    float FarWindowHeight = 0.0f;

    bool ViewDirty = true;

    // Cache View/Proj matrices.
    SMatrix View = SMatrix::Identity;
    SMatrix Proj = SMatrix::Identity;

    SMatrix PrevViewProj = SMatrix::Identity;
};
