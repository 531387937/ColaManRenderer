#include <windows.h>

#include "Actor/CameraActor.h"
#include "Actor/StaticMeshActor.h"
#include "Engine/Engine.h"

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
class CSampleWorld : public CWorld
{
public:
    CSampleWorld()
    {
        
    }
    
    virtual void InitWorld(CEngine* engine) override
    {
        CWorld::InitWorld(engine);
        auto Camera = AddActor<CCameraActor>("Camera");
        CameraComponent = Camera->GetCameraComponent();
        CameraComponent->SetWorldLocation(SVector3(2.0f, 2.0f, -10.0f));
        CameraComponent->UpdateViewMatrix();

        // Add CyborgWeapon
        // {
        //     auto CyborgWeapon = AddActor<CStaticMeshActor>("CyborgWeapon");
        //     CyborgWeapon->SetMesh("CyborgWeapon");
        //     CyborgWeapon->SetMaterialInstance("CyborgWeaponMatInst");
        //     CTransform Transform;
        //     Transform.Location = SVector3(-2.0f, 1.0f, -5.0f);
        //     Transform.Rotation = SRotator(0.0f, 0.0f, -45.0f);
        //     Transform.Scale = SVector3(300.0f, 300.0f, 300.0f);
        //     CyborgWeapon->SetActorTransform(Transform);
        // }
    }
    ~CSampleWorld() override
    {
    }
};

_Use_decl_annotations_

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
                   PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        {
            //_CrtSetBreakAlloc(550388);

            auto World = new CSampleWorld();

            SRenderSettings RenderSettings;
            RenderSettings.bEnableSSR = true;
            RenderSettings.bEnableTAA = true;
            RenderSettings.bDrawDebugText = true;

            CEngine Engine(hInstance);
            if (!Engine.Initialize(World, RenderSettings))
                return 0;

            Engine.Run();

            Engine.Destroy();
        }

        return 0;
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}
