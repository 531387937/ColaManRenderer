#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Component/Component.h"
#include "Math/Transform.h"

class CActor
{
public:
    CActor(const std::string& Name);

    virtual ~CActor()
    {
    }

public:
    virtual void Tick(float deltaTime)
    {
    }

public:
    template <typename T>
    T* AddComponent()
    {
        auto NewComponent = std::make_unique<T>();
        T* result = NewComponent.get();
        Components.push_back(std::move(NewComponent));

        return result;
    }

    std::vector<CComponent*> GetComponents()
    {
        std::vector<CComponent*> result;

        for (const auto& component : Components)
        {
            result.push_back(component.get());
        }

        return result;
    }

    template <typename T>
    std::vector<T*> GetComponentsOfClass()
    {
        std::vector<T*> result;
        for (const auto& component : Components)
        {
            T* componentOfClass = dynamic_cast<T*>(component.get());
            if (componentOfClass)
            {
                result.push_back(componentOfClass);
            }
        }

        return result;
    }

    CComponent* GetRootComponent() const
    {
        return RootComponent;
    }

    virtual void SetActorTransform(const CTransform& transform);

    CTransform GetActorTransform() const;

    void SetActorLocation(const SVector3& location);

    SVector3 GetActorLocation() const;

    void SetActorRotation(const SRotator& rotation);

    SRotator GetActorRotation() const;

    void SaveActorPreTransform();

    CTransform GetActorPrevTransform() const;

    void SetName(std::string name) { ActorName = name; }

    std::string GetName() const { return ActorName; }

protected:
    std::string ActorName;

    std::vector<std::unique_ptr<CComponent>> Components;

    CComponent* RootComponent = nullptr;
};
