#include "World.h"

#include <algorithm>

#include "Engine/Engine.h"

CWorld::CWorld()
{
}

void CWorld::InitWorld(CEngine* engine)
{
    Engine = engine;

    MainWindowHandle = engine->GetMainWnd();
}

std::string GetToggleStateStr(bool enable)
{
    if (enable)
    {
        return "ON";
    }
    return "OFF";
}

void CWorld::Update(const GameTimer& gt)
{
    OnKeyboardInput(gt);

    // Tick actors
    for (auto& Actor : Actors)
    {
        Actor->Tick(gt.DeltaTime());
    }

    // Calculate FPS and draw text
    {
        static float FPS = 0.0f;
        static float MSPF = 0.0f;

        static int FrameCnt = 0;
        static float TimeElapsed = 0.0f;
        FrameCnt++;

        // Compute averages over one second period.
        if ((gt.TotalTime() - TimeElapsed) >= 1.0f)
        {
            FPS = static_cast<float>(FrameCnt);
            MSPF = 1000.0f / FPS;

            // Reset for next average.
            FrameCnt = 0;
            TimeElapsed += 1.0f;
        }

        //DrawString(1, "FPS: " + std::to_string(FPS), 0.1f);
        //DrawString(2, "MSPF: " + std::to_string(MSPF), 0.1f);

        const SRenderSettings& RenderSettings = Engine->GetRender()->GetRenderSettings();

        // DrawString(3, "Hint: ", 0.1f);
        // DrawString(4, std::string("      Press H to toggle TAA [") + GetToggleStateStr(RenderSettings.bEnableTAA) +"]", 0.1f);
        // DrawString(5, std::string("      Press J to toggle SSR [") + GetToggleStateStr(RenderSettings.bEnableSSR) + "]", 0.1f);
        // DrawString(6, std::string("      Press K to toggle SSAO [") + GetToggleStateStr(RenderSettings.bEnableSSAO) + "]", 0.1f);
        // DrawString(7, std::string("      Press L to toggle SDF [") + GetToggleStateStr(RenderSettings.bDebugSDFScene) + "]", 0.1f);

        // Print camera message
        SVector3 CameraLocation = CameraComponent->GetWorldLocation();
        //DrawString(11, "CameraLocation(" + std::to_string(CameraLocation.x) + ", " + std::to_string(CameraLocation.y) + ", " + std::to_string(CameraLocation.z) + ")");
    }
}

void CWorld::EndFrame(const GameTimer& gt)
{
    SavePreFrameData();

    float deltaTime = gt.DeltaTime();

    Points.clear();

    Lines.clear();

    Triangles.clear();
}

void CWorld::SavePreFrameData()
{
    for (auto& actor : Actors)
    {
        if (actor->GetRootComponent())
        {
            actor->SaveActorPreTransform();
        }
    }

    SMatrix view = CameraComponent->GetView();
    SMatrix proj = CameraComponent->GetProj();
    SMatrix viewProj = view * proj;
    CameraComponent->SetPrevViewProj(viewProj);
}

void CWorld::OnMouseDown(WPARAM btnState, int x, int y)
{
    if ((btnState & MK_LBUTTON) != 0)
    {
        LastMousePos.x = x;
        LastMousePos.y = y;

        SetCapture(MainWindowHandle);
    }
}

void CWorld::OnMouseUp(WPARAM btnState, int x, int y)
{
    ReleaseCapture();
}

void CWorld::OnMouseMove(WPARAM btnState, int x, int y)
{
    if ((btnState & MK_LBUTTON) != 0)
    {
        // Make each pixel correspond to a quarter of a degree.
        float dx = 0.25f * static_cast<float>(x - LastMousePos.x);
        float dy = 0.25f * static_cast<float>(y - LastMousePos.y);

        CameraComponent->Pitch(dy);
        CameraComponent->RotateY(dx);
    }

    LastMousePos.x = x;
    LastMousePos.y = y;
}

void CWorld::OnMouseWheel(float WheelDistance)
{
    MoveSpeed += (WheelDistance / WHEEL_DELTA);

    MoveSpeed = std::clamp(MoveSpeed, 1.0f, 10.0f);
}

void CWorld::OnKeyboardInput(const GameTimer& gt)
{
    const float dt = gt.DeltaTime();

    /*-------------Camera-------------*/
    if (GetAsyncKeyState('W') & 0x8000)
        CameraComponent->MoveForward(MoveSpeed * dt);

    if (GetAsyncKeyState('S') & 0x8000)
        CameraComponent->MoveForward(-MoveSpeed * dt);

    if (GetAsyncKeyState('A') & 0x8000)
        CameraComponent->MoveRight(-MoveSpeed * dt);

    if (GetAsyncKeyState('D') & 0x8000)
        CameraComponent->MoveRight(MoveSpeed * dt);

    if (GetAsyncKeyState('Q') & 0x8000)
        CameraComponent->MoveUp(-MoveSpeed * dt);

    if (GetAsyncKeyState('E') & 0x8000)
        CameraComponent->MoveUp(MoveSpeed * dt);

    CameraComponent->UpdateViewMatrix();


    /*-------------Render settings---------------*/
    if (GetAsyncKeyState('H') & 0x8000)
    {
        if (!bKey_H_Pressed)
        {
            bKey_H_Pressed = true;
            Engine->GetRender()->ToggleTAA();
        }
    }
    else
    {
        bKey_H_Pressed = false;
    }

    if (GetAsyncKeyState('J') & 0x8000)
    {
        if (!bKey_J_Pressed)
        {
            bKey_J_Pressed = true;
            Engine->GetRender()->ToggleSSR();
        }
    }
    else
    {
        bKey_J_Pressed = false;
    }

    if (GetAsyncKeyState('K') & 0x8000)
    {
        if (!bKey_K_Pressed)
        {
            bKey_K_Pressed = true;
            Engine->GetRender()->ToggleSSAO();
        }
    }
    else
    {
        bKey_K_Pressed = false;
    }

    if (GetAsyncKeyState('L') & 0x8000)
    {
        if (!bKey_L_Pressed)
        {
            bKey_L_Pressed = true;
            //Engine->GetRender()->ToggleDebugSDF();
        }
    }
    else
    {
        bKey_L_Pressed = false;
    }
}

void CWorld::DrawPoint(const SVector3& point, const SColor& Color, int size)
{
    Points.emplace_back(point, Color);

    if (size != 0)
    {
        float Offset = 0.01f * size;

        Points.emplace_back(point + SVector3(Offset, 0.0f, 0.0f), Color);
        Points.emplace_back(point + SVector3(0.0f, Offset, 0.0f), Color);
        Points.emplace_back(point + SVector3(0.0f, 0.0f, Offset), Color);
        Points.emplace_back(point + SVector3(-Offset, 0.0f, 0.0f), Color);
        Points.emplace_back(point + SVector3(0.0f, -Offset, 0.0f), Color);
        Points.emplace_back(point + SVector3(0.0f, 0.0f, -Offset), Color);
    }
}

void CWorld::DrawPoint(const CPoint& point)
{
    Points.push_back(point);
}

const std::vector<CPoint>& CWorld::GetPoints()
{
    return Points;
}

void CWorld::DrawLine(const SVector3& pointA, const SVector3& pointB, const SColor& color)
{
    Lines.emplace_back(pointA, pointB, color);
}

void CWorld::DrawLine(const CLine& line)
{
    Lines.push_back(line);
}

const std::vector<CLine>& CWorld::GetLines()
{
    return Lines;
}

void CWorld::DrawBox3D(const SVector3& MinPointInWorld, const SVector3& MaxPointInWorld, const SColor& Color)
{
    SVector3 Min = MinPointInWorld;
    SVector3 Max = MaxPointInWorld;

    DrawLine(SVector3(Min.x, Min.y, Min.z), SVector3(Min.x, Min.y, Max.z), Color);
    DrawLine(SVector3(Min.x, Max.y, Min.z), SVector3(Min.x, Max.y, Max.z), Color);
    DrawLine(SVector3(Max.x, Min.y, Min.z), SVector3(Max.x, Min.y, Max.z), Color);
    DrawLine(SVector3(Max.x, Max.y, Min.z), SVector3(Max.x, Max.y, Max.z), Color);

    DrawLine(SVector3(Min.x, Min.y, Min.z), SVector3(Max.x, Min.y, Min.z), Color);
    DrawLine(SVector3(Min.x, Max.y, Min.z), SVector3(Max.x, Max.y, Min.z), Color);
    DrawLine(SVector3(Min.x, Min.y, Max.z), SVector3(Max.x, Min.y, Max.z), Color);
    DrawLine(SVector3(Min.x, Max.y, Max.z), SVector3(Max.x, Max.y, Max.z), Color);

    DrawLine(SVector3(Min.x, Min.y, Min.z), SVector3(Min.x, Max.y, Min.z), Color);
    DrawLine(SVector3(Max.x, Min.y, Min.z), SVector3(Max.x, Max.y, Min.z), Color);
    DrawLine(SVector3(Min.x, Min.y, Max.z), SVector3(Min.x, Max.y, Max.z), Color);
    DrawLine(SVector3(Max.x, Min.y, Max.z), SVector3(Max.x, Max.y, Max.z), Color);
}

void CWorld::DrawTriangle(const SVector3& PointAInWorld, const SVector3& PointBInWorld, const SVector3& PointCInWorld,
                          const SColor& Color)
{
    Triangles.emplace_back(PointAInWorld, PointBInWorld, PointCInWorld, Color);
}

void CWorld::DrawTriangle(const CTriangle& Triangle)
{
    Triangles.emplace_back(Triangle);
}

const std::vector<CTriangle>& CWorld::GetTriangles()
{
    return Triangles;
}
