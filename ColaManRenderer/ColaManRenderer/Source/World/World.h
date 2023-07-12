#pragma once
#include <memory>
#include <vector>

#include "Actor/Actor.h"
#include "Component/CameraComponent.h"
#include "Engine/Engine.h"
#include "Mesh/Primitive.h"

class CWorld
{
public:
    CWorld();

    virtual ~CWorld(){}

    virtual void InitWorld(CEngine* engine);

    virtual void Update(const GameTimer& gt);

    virtual void EndFrame(const GameTimer& gt);

private:
    void SavePreFrameData();

public:
    virtual void OnMouseDown(WPARAM btnState,int x,int y);
    
    virtual void OnMouseUp(WPARAM btnState, int x, int y);

    virtual void OnMouseMove(WPARAM btnState, int x, int y);

    virtual void OnMouseWheel(float WheelDistance);

    virtual void OnKeyboardInput(const GameTimer& gt);

    template<typename T>
    T* AddActor(const std::string& Name)
    {
        auto newActor = std::make_unique<T>(name);
        T* Result = newActor.get();
        Actors.push_back(std::move(newActor));

        return Result;
    }

    std::vector<CActor*> GetActors()
    {
        std::vector<CActor*> Result;
        for(const auto& actor:Actors)
        {
            Result.push_back(actor.get());
        }

        return Result;
    }

    template<typename T>
    std::vector<T*> GetAllActorsOfClass()
    {
        std::vector<T*> result;
        for(const auto& actor:Actors)
        {
            T* actorofClass = dynamic_cast<T*>(actor.get());
            if(actorofClass)
            {
                result.push_back(actorofClass);
            }
        }
        return result;
    }

    CCameraComponent* GetCameraComponent() {return CameraComponent;}

public:
    void DrawPoint(const SVector3& point,const SColor& Color,int size = 0);

    void DrawPoint(const CPoint& point);

    const std::vector<CPoint>& GetPoints();

    void DrawLine(const SVector3& pointA,const SVector3& pointB,const SColor& color);

    void DrawLine(const CLine& line);

    const std::vector<CLine>& GetLines();

    void DrawBox3D(const SVector3& MinPointInWorld, const SVector3& MaxPointInWorld, const SColor& Color);

    void DrawTriangle(const SVector3& PointAInWorld, const SVector3& PointBInWorld, const SVector3& PointCInWorld, const SColor& Color);

    void DrawTriangle(const CTriangle& Triangle);

    const std::vector<CTriangle>& GetTriangles();

    //void DrawSprite(const std::string& TextureName, const UIntPoint& TextureSize, const RECT& SourceRect, const RECT& DestRect);

   // const std::vector<TSprite>& GetSprites();

   // void DrawString(int ID, std::string Str, float Duration = 1.0f);

   // void GetTexts(std::vector<TText>& OutTexts);
protected:
    std::vector<std::unique_ptr<CActor>> Actors;

    std::vector<CPoint> Points;

    std::vector<CLine> Lines;

    std::vector<CTriangle> Triangles;

protected:
    CEngine* Engine = nullptr;

    HWND  MainWindowHandle = nullptr; // main window handle

    CCameraComponent* CameraComponent = nullptr;

    float MoveSpeed = 2.0f;

private:
    POINT LastMousePos;

    bool bKey_H_Pressed = false;

    bool bKey_J_Pressed = false;

    bool bKey_K_Pressed = false;

    bool bKey_L_Pressed = false;
};
