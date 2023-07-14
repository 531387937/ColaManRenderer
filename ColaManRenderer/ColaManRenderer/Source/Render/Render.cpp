#include "Render.h"

#include "RenderProxy.h"
#include "Actor/Light/DirectionalLightActor.h"
#include "Actor/Light/LightActor.h"
#include "Component/MeshComponent.h"
#include "D3D12/D3D12DescriptorCache.h"
#include "Mesh/MeshRepository.h"
#include "Texture/TextureRepository.h"

CRender::CRender()
{
}

CRender::~CRender()
{
    OnDestroy();
}

bool CRender::IsInitialize()
{
    return bInitialize;
}

bool CRender::Initialize(int windowWidth, int windowHeight, CD3D12RHI* d3d12RHI, CWorld* world,
                         const SRenderSettings& settings)
{
    D3D12RHI = d3d12RHI;

    World = world;

    RenderSetting = settings;

    D3DDevice = d3d12RHI->GetDevice()->GetD3DDevice();

    CommandList = d3d12RHI->GetDevice()->GetCommandList();

    GraphicsPSOManager = std::make_unique<CGraphicsPSOManager>(d3d12RHI, &InputLayoutManager);

    ComputePSOManager = std::make_unique<CComputePSOManager>(d3d12RHI);

    OnResize(windowWidth, windowHeight);

    CreateRenderResource();

    bInitialize = true;

    return true;
}

void CRender::OnResize(int Width, int Height)
{
    WindowHeight = Height;
    WindowWidth = Width;

    D3D12RHI->ResizeViewport(Width, Height);

    CCameraComponent* cameraCom = World->GetCameraComponent();
    cameraCom->SetLens(cameraCom->GetFovY(), AspectRatio(), cameraCom->GetNearZ(), cameraCom->GetFarZ());

    //TODO Should collected all of the Pass
    CreateGBuffers();

    CreateSSAOBuffer();

    CreateBackDepth();

    CreateColorTextures();

    CreateComputeShaderResource();
}

void CRender::Draw(const GameTimer& gt)
{
    D3D12RHI->ResetCommandAllocator();

    D3D12RHI->ResetCommandList();

    SetDescriptorHeaps();

    //TODO Add IBL

    GatherAllMeshBatches();

    UpdateLightData();

    D3D12RHI->ExecuteCommandLists();

    D3D12RHI->Present();

    D3D12RHI->FlushCommandQueue();
}

void CRender::EndFrame()
{
    D3D12RHI->EndFrame();

    FrameCount++;
}

void CRender::OnDestroy()
{
    D3D12RHI->FlushCommandQueue();
}

float CRender::AspectRatio() const
{
    return static_cast<float>(WindowWidth) / (WindowHeight);
}

CD3D12Resource* CRender::CurrentBackBuffer() const
{
    return D3D12RHI->GetViewport()->GetCurrentBackBuffer();
}

D3D12_CPU_DESCRIPTOR_HANDLE CRender::CurrentBackBufferView() const
{
    return D3D12RHI->GetViewport()->GetCurrentBackBufferView()->GetDescriptorHandle();
}

float* CRender::CurrentBackBufferClearColor() const
{
    return D3D12RHI->GetViewport()->GetCurrentBackBufferClearColor();
}

D3D12_CPU_DESCRIPTOR_HANDLE CRender::DepthStencilView() const
{
    return D3D12RHI->GetViewport()->GetDepthStencilView()->GetDescriptorHandle();
}

void CRender::CreateRenderResource()
{
    D3D12RHI->ResetCommandList();

    CreateNullDescriptors();

    CreateTextures();

    GetSkyInfo();

    if (SkyMeshComponent)
    {
        bEnableIBLEnvLighting = true;

        CreateSceneCaptureCube();
    }

    CreateMeshProxys();

    CreateInputLayouts();

    CreateGlobalShaders();

    CreateGlobalPSO();

    CreateComputePSO();

    D3D12RHI->ExecuteCommandLists();

    D3D12RHI->FlushCommandQueue();
}

void CRender::CreateNullDescriptors()
{
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
        SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

        Texture2DNullDescriptor = std::make_unique<CD3D12ShaderResourceView>(D3D12RHI->GetDevice(), SrvDesc, nullptr);
    }

    {
        D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
        SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;

        Texture3DNullDescriptor = std::make_unique<CD3D12ShaderResourceView>(D3D12RHI->GetDevice(), SrvDesc, nullptr);
    }

    {
        D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
        SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;

        TextureCubeNullDescriptor = std::make_unique<CD3D12ShaderResourceView>(D3D12RHI->GetDevice(), SrvDesc, nullptr);
    }

    {
        D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
        SrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        SrvDesc.Format = DXGI_FORMAT_UNKNOWN;
        SrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        SrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        SrvDesc.Buffer.StructureByteStride = 1;
        SrvDesc.Buffer.NumElements = 1;
        SrvDesc.Buffer.FirstElement = 0;

        StructuredBufferNullDescriptor = std::make_unique<CD3D12ShaderResourceView>(
            D3D12RHI->GetDevice(), SrvDesc, nullptr);
    }
}

void CRender::CreateTextures()
{
    const auto& TextureMap = CTextureRepository::Get().TextureMap;

    for (const auto& texture : TextureMap)
    {
        texture.second->LoadTextureResourceFromFlie(D3D12RHI);
        texture.second->CreateTexture(D3D12RHI);
    }
}

void CRender::CreateSceneCaptureCube()
{
    IBLEnvironmentMap = std::make_unique<CSceneCaptureCube>(false, 512, DXGI_FORMAT_R32G32B32_FLOAT, D3D12RHI);
    IBLEnvironmentMap->CreatePerspectiveViews({0.0f, 0.0f, 0.0f}, 0.1f, 10.0f);
}

void CRender::CreateGBuffers()
{
}

void CRender::CreateSSAOBuffer()
{
}

void CRender::CreateBackDepth()
{
}

void CRender::CreateColorTextures()
{
}

void CRender::CreateMeshProxys()
{
}

void CRender::CreateInputLayouts()
{
}

void CRender::CreateGlobalShaders()
{
}

void CRender::CreateGlobalPSO()
{
}

void CRender::CreateComputePSO()
{
}

void CRender::CreateComputeShaderResource()
{
}

void CRender::SetDescriptorHeaps()
{
    auto CacheCbvSrvUavDescriptorHeap = D3D12RHI->GetDevice()->GetCommandContext()->GetDescriptorCache()->
                                                  GetCacheCbvSrvUavDescriptorHeap();
    ID3D12DescriptorHeap* descriptorHeaps[] = {CacheCbvSrvUavDescriptorHeap.Get()};

    CommandList->SetDescriptorHeaps(1, descriptorHeaps);
}

void CRender::GetSkyInfo()
{
}

void CRender::UpdateIBLEnvironmentPassCB()
{
}

void CRender::CreateIBLEnvironmentMap()
{
}

void CRender::UpdateIBLIrradiancePassCB()
{
}

void CRender::CreateIBLIrradianceMap()
{
}

void CRender::UpdateIBLPrefilterEnvCB()
{
}

void CRender::CreateIBLPrefilterEnvMap()
{
}

void CRender::GatherAllMeshBatches()
{
    MeshBatches.clear();

    auto actors = World->GetActors();
    std::vector<CMeshComponent*> allMeshComponents;

    for (auto actor : actors)
    {
        auto light = dynamic_cast<CLightActor*>(actor);
        if (light && !light->IsDrawDebug())
        {
            continue;
        }

        auto meshComponents = actor->GetComponentsOfClass<CMeshComponent>();
        for (auto mesh : meshComponents)
        {
            if (mesh->IsMeshValid())
            {
                allMeshComponents.push_back(mesh);
            }
        }
    }

    CCameraComponent* cameraComponent = World->GetCameraComponent();
    SMatrix invVeiw = cameraComponent->GetView().Invert();

    //Frustum Culling
    DirectX::BoundingFrustum viewSpaceFrustum;
    DirectX::BoundingFrustum::CreateFromMatrix(viewSpaceFrustum, cameraComponent->GetProj());

    std::vector<CMeshComponent*> meshComponentsAfterCulling;
    for (auto mesh : allMeshComponents)
    {
        SMatrix invWorld = mesh->GetWorldTransform().GetTransformMatrix().Invert();
        SMatrix invMV = invVeiw * invWorld;

        DirectX::BoundingFrustum localSpaceFrustum;

        viewSpaceFrustum.Transform(localSpaceFrustum, invMV);

        CBoundingBox boundingBox;
        if (bEnableFrustumCulling && mesh->GetLocalBoundingBox(boundingBox))
        {
            if (localSpaceFrustum.Contains(boundingBox.GetD3DBox()) != DirectX::DISJOINT)
            {
                meshComponentsAfterCulling.push_back(mesh);
            }
        }
        else
        {
            meshComponentsAfterCulling.push_back(mesh);
        }
    }

    //Generate MeshBatches
    for (auto mesh : meshComponentsAfterCulling)
    {
        std::string meshName = mesh->GetMeshName();

        SMeshBatch meshBatch;
        meshBatch.MeshName = meshName;
        meshBatch.InputLayoutName = CMeshRepository::Get().MeshMap.at(meshName).GetInputLayoutName();

        meshBatch.MeshComponent = mesh;

        MeshBatches.emplace_back(meshBatch);
    }
}

SMatrix CRender::TextureTransform()
{
    return SMatrix::Identity;
}

void CRender::UpdateLightData()
{
    std::vector<SLightShaderParameters> lightShaderParametersArray;

    ShadowMapSRVs.clear();
    ShadowMapCubeSRVs.clear();

    auto lights = World->GetAllActorsOfClass<CLightActor>();

    for (UINT lightIndex = 0; lightIndex < lights.size(); lightIndex++)
    {
        auto light = lights[lightIndex];

        if (light->GetType() == ELightType::DirectionalLight)
        {
            auto directLight = dynamic_cast<CDirectionalLightActor*>(light);
            assert(directLight);

            SLightShaderParameters lightShaderParameter;
            lightShaderParameter.Color = directLight->GetLightColor();
            lightShaderParameter.Direction = directLight->GetLightDirection();
            lightShaderParameter.Intensity = directLight->GetLightIntensity();
            lightShaderParameter.Position = directLight->GetActorLocation();
            lightShaderParameter.LightType = static_cast<UINT>(ELightType::DirectionalLight);
        }
    }
}

void CRender::UpdateShadowPassCB(const SSceneView& sceneView, UINT shadowWidth, UINT ShadowHeight)
{
}

void CRender::GetShadowPassMeshCommandMap(EShadowMapType Type)
{
}

void CRender::ShadowPass()
{
}

void CRender::GenerateSingleShadowMap(CShadowMap2D* shadowMap)
{
}

void CRender::GenerateOmniShadowMap(CShadowMapCube* shadowMap)
{
}

void CRender::UpdateBasePassCB()
{
}

void CRender::GetBasePassMeshCommandMap()
{
}

void CRender::BasePass()
{
}

void CRender::GetBackDepthPassMeshCommandMap()
{
}

void CRender::BackDepthPass()
{
}

void CRender::UpdateSSAOPassCB()
{
}

void CRender::SSAOPass()
{
}

void CRender::GatherLightDebugPrimitives(std::vector<CLine>& outLines)
{
}

void CRender::GatherAllPrimitiveBatches()
{
}

void CRender::GatherPrimitiveBatches(const std::vector<SPrimitiveVertex>& vertices,
                                     D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType)
{
}

void CRender::PrimitivesPass()
{
}

void CRender::UpdateDefferredLightingPassCB()
{
}

void CRender::DefferredLightingPass()
{
}

void CRender::UpdateSSRPassCB()
{
}

void CRender::SSRPass()
{
}

void CRender::TAAPass()
{
}

void CRender::PostProcessPass()
{
}

const SRenderSettings& CRender::GetRenderSettings()
{
    return RenderSetting;
}

void CRender::ToggleTAA()
{
}

void CRender::ToggleSSR()
{
}

void CRender::ToggleSSAO()
{
}
