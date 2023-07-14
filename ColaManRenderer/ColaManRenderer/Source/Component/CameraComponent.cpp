#include "CameraComponent.h"

CCameraComponent::CCameraComponent()
{
    SetLens(0.25f * CMath::Pi, 1.0f, 0.1f, 100.0f);
}

CCameraComponent::~CCameraComponent()
{
}

void CCameraComponent::SetWorldLocation(const SVector3& Location)
{
    WorldTransform.Location = Location;
    ViewDirty = true;
}

float CCameraComponent::GetNearZ() const
{
    return NearZ;
}

float CCameraComponent::GetFarZ() const
{
    return FarZ;
}

float CCameraComponent::GetAspect() const
{
    return Aspect;
}

float CCameraComponent::GetFovY() const
{
    return FovY;
}

float CCameraComponent::GetFovX() const
{
    float halfWidth = 0.5f * GetNearWindowWidth();
    return static_cast<float>(2.0f * atan(halfWidth / NearZ));
}

float CCameraComponent::GetNearWindowWidth() const
{
    return Aspect * NearWindowHeight;
}

float CCameraComponent::GetNearWindowHeight() const
{
    return NearWindowHeight;
}

float CCameraComponent::GetFarWindowWidth() const
{
    return Aspect * FarWindowHeight;
}

float CCameraComponent::GetFarWindowHeight() const
{
    return FarWindowHeight;
}

void CCameraComponent::SetLens(float fovY, float aspect, float zn, float zf)
{
    // cache properties
    FovY = fovY;
    Aspect = aspect;
    NearZ = zn;
    FarZ = zf;

    NearWindowHeight = 2.0f * NearZ * tanf(0.5f * FovY);
    FarWindowHeight = 2.0f * FarZ * tanf(0.5f * FovY);

    Proj = SMatrix::CreatePerspectiveFieldOfView(FovY, Aspect, NearZ, FarZ);
}

void CCameraComponent::LookAt(const SVector3& pos, const SVector3& target, const SVector3& up)
{
    SVector3 L = target - pos;
    L.Normalize();
    SVector3 R = up.Cross(L);
    R.Normalize();
    SVector3 U = L.Cross(R);

    WorldTransform.Location = pos;
    Look = L;
    Right = R;
    Up = U;

    ViewDirty = true;
}


SMatrix CCameraComponent::GetView() const
{
    assert(!ViewDirty);
    return View;
}

SMatrix CCameraComponent::GetProj() const
{
    return Proj;
}

void CCameraComponent::MoveRight(float Dist)
{
    WorldTransform.Location += Dist * Right;

    ViewDirty = true;
}

void CCameraComponent::MoveForward(float Dist)
{
    WorldTransform.Location += Dist * Look;

    ViewDirty = true;
}

void CCameraComponent::MoveUp(float Dist)
{
    WorldTransform.Location += Dist * Up;

    ViewDirty = true;
}

void CCameraComponent::Pitch(float Degrees)
{
    float Radians = CMath::DegreesToRadians(Degrees);

    // Rotate up and look vector about the right vector.
    SMatrix R = SMatrix::CreateFromAxisAngle(Right, Radians);

    Up = R.TransformNormal(Up);
    Look = R.TransformNormal(Look);

    ViewDirty = true;
}

void CCameraComponent::RotateY(float Degrees)
{
    float Radians = CMath::DegreesToRadians(Degrees);

    // Rotate the basis vectors about the world y-axis.
    SMatrix R = SMatrix::CreateRotationY(Radians);

    Right = R.TransformNormal(Right);
    Up = R.TransformNormal(Up);
    Look = R.TransformNormal(Look);

    ViewDirty = true;
}

void CCameraComponent::UpdateViewMatrix()
{
    if (ViewDirty)
    {
        // Keep camera's axes orthogonal to each other and of unit length.
        Look.Normalize();
        Up = Look.Cross(Right);
        Up.Normalize();

        // Up, Look already ortho-normal, so no need to normalize cross product.
        Right = Up.Cross(Look);

        // Fill in the view matrix entries.
        float x = -WorldTransform.Location.Dot(Right);
        float y = -WorldTransform.Location.Dot(Up);
        float z = -WorldTransform.Location.Dot(Look);

        View(0, 0) = Right.x;
        View(1, 0) = Right.y;
        View(2, 0) = Right.z;
        View(3, 0) = x;

        View(0, 1) = Up.x;
        View(1, 1) = Up.y;
        View(2, 1) = Up.z;
        View(3, 1) = y;

        View(0, 2) = Look.x;
        View(1, 2) = Look.y;
        View(2, 2) = Look.z;
        View(3, 2) = z;

        View(0, 3) = 0.0f;
        View(1, 3) = 0.0f;
        View(2, 3) = 0.0f;
        View(3, 3) = 1.0f;

        ViewDirty = false;
    }
}

void CCameraComponent::SetPrevViewProj(const SMatrix& VP)
{
    PrevViewProj = VP;
}

SMatrix CCameraComponent::GetPrevViewProj() const
{
    return PrevViewProj;
}
