#include "Actor.h"

CActor::CActor(const std::string& Name)
{
    SetName(Name);
}

void CActor::SetActorTransform(const CTransform& transform)
{
    RootComponent->SetWorldTransform(transform);
}

CTransform CActor::GetActorTransform() const
{
    return RootComponent->GetWorldTransform();
}

void CActor::SetActorLocation(const SVector3& location)
{
    RootComponent->SetWorldLocation(location);
}

SVector3 CActor::GetActorLocation() const
{
    return RootComponent->GetWorldLocation();
}

void CActor::SetActorRotation(const SRotator& rotation)
{
    RootComponent->SetWorldRotation(rotation);
}

SRotator CActor::GetActorRotation() const
{
    return RootComponent->GetWorldRotation();
}

void CActor::SaveActorPreTransform()
{
    RootComponent->SetPrevWorldTransform(RootComponent->GetWorldTransform());
}

CTransform CActor::GetActorPrevTransform() const
{
    return RootComponent->GetPrevWorldTransform();
}
