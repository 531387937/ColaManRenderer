#include "MeshRepository.h"

CMeshRepository::CMeshRepository()
{
    //FbxLoader = std::make_unique<TFbxLoader>();
    //FbxLoader->Init();
}

CMeshRepository& CMeshRepository::Get()
{
    static CMeshRepository Instance;
    return Instance;
}

void CMeshRepository::Load()
{
    CMesh BoxMesh;
    BoxMesh.CreateBox(1.0f, 1.0f, 1.0f, 3);
    BoxMesh.MeshName = "BoxMesh";
    BoxMesh.GenerateBoundingBox();
    MeshMap.emplace("BoxMesh", std::move(BoxMesh));

    CMesh SphereMesh;
    SphereMesh.CreateSphere(0.5f, 20, 20);
    SphereMesh.MeshName = "SphereMesh";
    SphereMesh.GenerateBoundingBox();
    MeshMap.emplace("SphereMesh", std::move(SphereMesh));

    CMesh CylinderMesh;
    CylinderMesh.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
    CylinderMesh.MeshName = "CylinderMesh";
    CylinderMesh.GenerateBoundingBox();
    MeshMap.emplace("CylinderMesh", std::move(CylinderMesh));

    CMesh GridMesh;
    GridMesh.CreateGrid(20.0f, 30.0f, 60, 40);
    GridMesh.MeshName = "GridMesh";
    GridMesh.GenerateBoundingBox();
    MeshMap.emplace("GridMesh", std::move(GridMesh));

    CMesh QuadMesh;
    QuadMesh.CreateQuad(-0.5f, 0.5f, 1.0f, 1.0f, 0.0f);
    QuadMesh.MeshName = "QuadMesh";
    QuadMesh.GenerateBoundingBox();
    MeshMap.emplace("QuadMesh", std::move(QuadMesh));

    CMesh ScreenQuadMesh;
    ScreenQuadMesh.CreateQuad(-1.0f, 1.0f, 2.0f, 2.0f, 0.0f);
    ScreenQuadMesh.MeshName = "ScreenQuadMesh";
    ScreenQuadMesh.GenerateBoundingBox();
    MeshMap.emplace("ScreenQuadMesh", std::move(ScreenQuadMesh));

    CMesh AssaultRifleMesh;
    AssaultRifleMesh.MeshName = "AssaultRifle";
    //FbxLoader->LoadFBXMesh(L"LOW_WEPON.fbx", AssaultRifleMesh);
    AssaultRifleMesh.GenerateBoundingBox();
    MeshMap.emplace("AssaultRifle", std::move(AssaultRifleMesh));

    CMesh CyborgWeaponMesh;
    CyborgWeaponMesh.MeshName = "CyborgWeapon";
    //FbxLoader->LoadFBXMesh(L"Cyborg_Weapon.fbx", CyborgWeaponMesh);
    CyborgWeaponMesh.GenerateBoundingBox();
    MeshMap.emplace("CyborgWeapon", std::move(CyborgWeaponMesh));

    CMesh HelmetMesh;
    HelmetMesh.MeshName = "Helmet";
    //FbxLoader->LoadFBXMesh(L"helmet_low.fbx", HelmetMesh);
    HelmetMesh.GenerateBoundingBox();
    MeshMap.emplace("Helmet", std::move(HelmetMesh));

    CMesh ColumnMesh;
    ColumnMesh.MeshName = "Column";
    //FbxLoader->LoadFBXMesh(L"column.fbx", ColumnMesh);
    ColumnMesh.GenerateBoundingBox();
    MeshMap.emplace("Column", std::move(ColumnMesh));
}

void CMeshRepository::Unload()
{
    MeshMap.clear();
}
