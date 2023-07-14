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
    //TODO 创建一个渲染队列，其中每个PASS都有渲染优先级，根据优先级渲染，每个Pass都继承自一个RenderPass基类，调用Draw函数
    D3D12RHI->ResetCommandAllocator();

    D3D12RHI->ResetCommandList();

    SetDescriptorHeaps();

    //TODO Add IBL

    GatherAllMeshBatches();

    UpdateLightData();

    BasePass();

	PrimitivesPass();
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
    GBufferBaseColor = std::make_unique<CRenderTarget2D>(D3D12RHI, false, WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);

    GBufferNormal = std::make_unique<CRenderTarget2D>(D3D12RHI, false, WindowWidth, WindowHeight, DXGI_FORMAT_R8G8B8A8_SNORM);

    GBufferWorldPos = std::make_unique<CRenderTarget2D>(D3D12RHI, false, WindowWidth, WindowHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);

    GBufferORM = std::make_unique<CRenderTarget2D>(D3D12RHI, false, WindowWidth, WindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

    GBufferVelocity = std::make_unique<CRenderTarget2D>(D3D12RHI, false, WindowWidth, WindowHeight, DXGI_FORMAT_R16G16_FLOAT);

    GBufferEmissive = std::make_unique<CRenderTarget2D>(D3D12RHI, false, WindowWidth, WindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
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
	return;
    auto& meshMap = CMeshRepository::Get().MeshMap;

    for(auto& meshpair:meshMap)
    {
        CMesh& mesh = meshpair.second;

        MeshProxyMap.emplace(mesh.MeshName,SMeshProxy());
        SMeshProxy& meshProxy = MeshProxyMap.at(mesh.MeshName);

        const UINT vbByteSize = (UINT)mesh.Vertices.size()*sizeof(SVertex);

        std::vector<std::uint16_t> indices = mesh.GetIndices16();
        const UINT ibByteSize = (UINT)indices.size()*sizeof(std::uint16_t);

        meshProxy.VertexBufferRef = D3D12RHI->CreateVertexBuffer(mesh.Vertices.data(),vbByteSize);
        meshProxy.IndexBufferRef = D3D12RHI->CreateIndexBuffer(indices.data(),ibByteSize);

        meshProxy.VertexByteStride = sizeof(SVertex);
        meshProxy.VertexBufferByteSize = vbByteSize;
        meshProxy.IndexFormat = DXGI_FORMAT_R16_UINT;
        meshProxy.IndexBufferByteSize = ibByteSize;

        SSubmeshProxy subMesh;
        subMesh.IndexCount = (UINT)indices.size();
        subMesh.StartIndexLocation = 0;
        subMesh.BaseVertexLocation = 0;

        meshProxy.SubMeshes["Defaulot"] = subMesh;
    }
}

void CRender::CreateInputLayouts()
{
    std::vector<D3D12_INPUT_ELEMENT_DESC>  DefaultInputLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    InputLayoutManager.AddInputLayout("DefaultInputLayout", DefaultInputLayout);


    //PositionColorInputLayout
    std::vector<D3D12_INPUT_ELEMENT_DESC>  PositionColorInputLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    InputLayoutManager.AddInputLayout("PositionColorInputLayout", PositionColorInputLayout);

    //PositionTexcoordInputLayout
    std::vector<D3D12_INPUT_ELEMENT_DESC> PositionTexcoordInputLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    InputLayoutManager.AddInputLayout("PositionTexcoordInputLayout", PositionTexcoordInputLayout);
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
    CCameraComponent* camera = World->GetCameraComponent();

    SMatrix view = camera->GetView();
    SMatrix proj = camera->GetProj();
    SMatrix viewProj = view*proj;
    SMatrix invView = view.Invert();
    SMatrix invProj = proj.Invert();
    SMatrix invViewProj = viewProj.Invert();
    SMatrix preViewProj = camera->GetPrevViewProj();

    SPassConstants basePassCB;
    basePassCB.View = view.Transpose();
    basePassCB.Proj = proj.Transpose();
    basePassCB.ViewProj = viewProj.Transpose();
    basePassCB.InvView = invView.Transpose();
    basePassCB.InvProj = invProj.Transpose();
    basePassCB.InvViewProj = invViewProj.Transpose();
    basePassCB.PrevViewProj = preViewProj.Transpose();
    basePassCB.EyePosW = camera->GetWorldLocation();
    basePassCB.NearZ = camera->GetNearZ();
    basePassCB.FarZ = camera->GetFarZ();
    basePassCB.RenderTargetSize = SVector2(WindowWidth,WindowHeight);
    basePassCB.InvRenderTargetSize = SVector2(1.0f/WindowWidth,1.0f/WindowHeight);

    BasePassCBRef = D3D12RHI->CreateConstantBuffer(&basePassCB,sizeof(SPassConstants));
}

void CRender::GetBasePassMeshCommandMap()
{
    BaseMeshCommandMap.clear();

    for(const SMeshBatch& meshBatch:MeshBatches)
    {
        SMeshCommand meshCommand;
        meshCommand.MeshName = meshBatch.MeshName;

        auto MaterialInstance = meshBatch.MeshComponent->GetMaterialInstance();
        meshCommand.RenderState = MaterialInstance->Material->RenderState;

        if(!MaterialInstance->MaterialConstantBuffer)
        {
            MaterialInstance->CreateMaterialConstantBuffer(D3D12RHI);
        }

        meshCommand.SetShaderParameter("cbMaterialData",MaterialInstance->MaterialConstantBuffer);
        meshCommand.SetShaderParameter("cbPass",BasePassCBRef);
        meshCommand.SetShaderParameter("cbPerObject",meshBatch.ObjConstantBuffer);
        for(const auto& item:MaterialInstance->Parameters.TextureMap)
        {
            std::string textureName = item.second;
            CD3D12ShaderResourceView* SRV = nullptr;

            if (textureName == SkyCubeTextureName)
            {
                continue;
                SRV = IBLEnvironmentMap->GetRTCube()->GetSRV();
            }
            else
            {
                SRV = CTextureRepository::Get().TextureMap[textureName]->GetD3DTexture()->GetSRV();
            }
            meshCommand.SetShaderParameter(item.first,SRV);
        }
        SGraphicsPSODescriptor Descriptor;
        Descriptor.InputLayoutName = meshBatch.InputLayoutName;
        Descriptor.RasterizerDesc.CullMode = meshCommand.RenderState.CullMode;
        Descriptor.DepthStencilDesc.DepthFunc = meshCommand.RenderState.DepthFunc;

        CMaterial* Material = MaterialInstance->Material;
        SShaderDefines EmptyShaderDefines;
        Descriptor.Shader = Material->GetShader(EmptyShaderDefines, D3D12RHI);
		
        // GBuffer PSO common settings
        Descriptor.RTVFormats[0] = GBufferBaseColor->GetFormat();
        Descriptor.RTVFormats[1] = GBufferNormal->GetFormat();
        Descriptor.RTVFormats[2] = GBufferWorldPos->GetFormat();
        Descriptor.RTVFormats[3] = GBufferORM->GetFormat();
        Descriptor.RTVFormats[4] = GBufferVelocity->GetFormat();
        Descriptor.RTVFormats[5] = GBufferEmissive->GetFormat();
        Descriptor.NumRenderTargets = GBufferCount;
        Descriptor.DepthStencilFormat = D3D12RHI->GetViewportInfo().DepthStencilFormat;
        Descriptor._4xMsaaState = false; //can't use msaa in deferred rendering.

        // Create a new PSO if we don't have the pso with this descriptor
        GraphicsPSOManager->CreatePSO(Descriptor);

        // Save MeshCommand according to PSO type
        BaseMeshCommandMap.insert({ Descriptor, CMeshCommandList() });
        BaseMeshCommandMap[Descriptor].emplace_back(meshCommand);
    }
}

void CRender::BasePass()
{
    UpdateBasePassCB();

    GetBasePassMeshCommandMap();

    D3D12_VIEWPORT ScreenViewport;
	D3D12_RECT ScissorRect;
	D3D12RHI->GetViewport()->GetD3DViewport(ScreenViewport, ScissorRect);
	CommandList->RSSetViewports(1, &ScreenViewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);

	// Transit to render target state
	D3D12RHI->TransitionResource(GBufferBaseColor->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12RHI->TransitionResource(GBufferNormal->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12RHI->TransitionResource(GBufferWorldPos->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12RHI->TransitionResource(GBufferORM->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12RHI->TransitionResource(GBufferVelocity->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12RHI->TransitionResource(GBufferEmissive->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Clear renderTargets
	CommandList->ClearRenderTargetView(GBufferBaseColor->GetRTV()->GetDescriptorHandle(), GBufferBaseColor->GetClearColorPtr(), 0, nullptr);
	CommandList->ClearRenderTargetView(GBufferNormal->GetRTV()->GetDescriptorHandle(), GBufferNormal->GetClearColorPtr(), 0, nullptr);
	CommandList->ClearRenderTargetView(GBufferWorldPos->GetRTV()->GetDescriptorHandle(), GBufferWorldPos->GetClearColorPtr(), 0, nullptr);
	CommandList->ClearRenderTargetView(GBufferORM->GetRTV()->GetDescriptorHandle(), GBufferORM->GetClearColorPtr(), 0, nullptr);
	CommandList->ClearRenderTargetView(GBufferVelocity->GetRTV()->GetDescriptorHandle(), GBufferVelocity->GetClearColorPtr(), 0, nullptr);
	CommandList->ClearRenderTargetView(GBufferEmissive->GetRTV()->GetDescriptorHandle(), GBufferEmissive->GetClearColorPtr(), 0, nullptr);

	// Clear depthstencil
	CommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the renderTargets we are going to render to.
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RtvDescriptors;
	RtvDescriptors.push_back(GBufferBaseColor->GetRTV()->GetDescriptorHandle());
	RtvDescriptors.push_back(GBufferNormal->GetRTV()->GetDescriptorHandle());
	RtvDescriptors.push_back(GBufferWorldPos->GetRTV()->GetDescriptorHandle());
	RtvDescriptors.push_back(GBufferORM->GetRTV()->GetDescriptorHandle());
	RtvDescriptors.push_back(GBufferVelocity->GetRTV()->GetDescriptorHandle());
	RtvDescriptors.push_back(GBufferEmissive->GetRTV()->GetDescriptorHandle());

	auto DescriptorCache = D3D12RHI->GetDevice()->GetCommandContext()->GetDescriptorCache();
	CD3DX12_GPU_DESCRIPTOR_HANDLE GpuHandle;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuHandle;
	DescriptorCache->AppendRtvDescriptors(RtvDescriptors, GpuHandle, CpuHandle);

	CommandList->OMSetRenderTargets(GBufferCount, &CpuHandle, true, &DepthStencilView());

	// Draw all mesh
	for (const auto& Pair : BaseMeshCommandMap)
	{
		const SGraphicsPSODescriptor& PSODescriptor = Pair.first;
		const CMeshCommandList& MeshCommandList = Pair.second;

		// Set PSO
		CommandList->SetPipelineState(GraphicsPSOManager->GetPSO(PSODescriptor));

		// Set RootSignature
		CShader* Shader = PSODescriptor.Shader;
		CommandList->SetGraphicsRootSignature(Shader->RootSignature.Get()); //should before binding

		for (const SMeshCommand& MeshCommand : MeshCommandList)
		{
			auto& TextureMap = CTextureRepository::Get().TextureMap;

			// Set paramters
			MeshCommand.ApplyShaderParamters(Shader);

			// Bind paramters
			Shader->BindParameters();

			const SMeshProxy& MeshProxy = MeshProxyMap.at(MeshCommand.MeshName);

			// Set vertex buffer
			D3D12RHI->SetVertexBuffer(MeshProxy.VertexBufferRef, 0, MeshProxy.VertexByteStride, MeshProxy.VertexBufferByteSize);

			// Set index buffer
			D3D12RHI->SetIndexBuffer(MeshProxy.IndexBufferRef, 0, MeshProxy.IndexFormat, MeshProxy.IndexBufferByteSize);

			D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			CommandList->IASetPrimitiveTopology(PrimitiveType);

			// Draw 
			auto& SubMesh = MeshProxy.SubMeshes.at("Default");
			CommandList->DrawIndexedInstanced(SubMesh.IndexCount, 1, SubMesh.StartIndexLocation, SubMesh.BaseVertexLocation, 0);
		}
	}

	// Transit to generic read state
	D3D12RHI->TransitionResource(GBufferBaseColor->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	D3D12RHI->TransitionResource(GBufferNormal->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	D3D12RHI->TransitionResource(GBufferWorldPos->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	D3D12RHI->TransitionResource(GBufferORM->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	D3D12RHI->TransitionResource(GBufferVelocity->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	D3D12RHI->TransitionResource(GBufferEmissive->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
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
	PSOPrimitiveBatchMap.clear();

	// Points
	{
		// Get all points
		std::vector<SPrimitiveVertex> Vertices;
		const auto& Points = World->GetPoints();
		for (const auto& Point : Points)
		{
			Vertices.push_back(SPrimitiveVertex(Point.Point, Point.Color));
		}

		GatherPrimitiveBatches(Vertices, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
	}

	// Lines
	{
		// Get all lines
		std::vector<CLine> Lines;
		const auto& WorldLines = World->GetLines();
		Lines.insert(Lines.end(), WorldLines.begin(), WorldLines.end());
		
		std::vector<CLine> LightDebugLines;
		GatherLightDebugPrimitives(LightDebugLines);
		Lines.insert(Lines.end(), LightDebugLines.begin(), LightDebugLines.end());

		std::vector<SPrimitiveVertex> Vertices;
		for (const auto& Line : Lines)
		{
			Vertices.push_back(SPrimitiveVertex(Line.PointA, Line.Color));
			Vertices.push_back(SPrimitiveVertex(Line.PointB, Line.Color));
		}

		GatherPrimitiveBatches(Vertices, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	}


	// Triangles
	{
		// Get all triangles
		std::vector<SPrimitiveVertex> Vertices;
		const auto& Triangles = World->GetTriangles();
		for (const auto& Triangle : Triangles) {
			Vertices.push_back(SPrimitiveVertex(Triangle.PointA, Triangle.Color));
			Vertices.push_back(SPrimitiveVertex(Triangle.PointB, Triangle.Color));
			Vertices.push_back(SPrimitiveVertex(Triangle.PointC, Triangle.Color));
		}

		GatherPrimitiveBatches(Vertices, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	}
}

void CRender::GatherPrimitiveBatches(const std::vector<SPrimitiveVertex>& vertices,
                                     D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType)
{
	{
		SShaderInfo ShaderInfo;
		ShaderInfo.ShaderName = "Primitive";
		ShaderInfo.FileName = "Primitive";
		ShaderInfo.bCreateVS = true;
		ShaderInfo.bCreatePS = true;
		PrimitiveShader = std::make_unique<CShader>(ShaderInfo, D3D12RHI);
	}
    // Primitive PSO
	SGraphicsPSODescriptor PSODescriptor;
	PSODescriptor.InputLayoutName = std::string("PositionColorInputLayout");
	PSODescriptor.Shader = PrimitiveShader.get();
	PSODescriptor.PrimitiveTopologyType = primitiveType;

	// GBuffer PSO common settings
	PSODescriptor.RTVFormats[0] = GBufferBaseColor->GetFormat();
	PSODescriptor.RTVFormats[1] = GBufferNormal->GetFormat();
	PSODescriptor.RTVFormats[2] = GBufferWorldPos->GetFormat();
	PSODescriptor.RTVFormats[3] = GBufferORM->GetFormat();
	PSODescriptor.RTVFormats[4] = GBufferVelocity->GetFormat();
	PSODescriptor.RTVFormats[5] = GBufferEmissive->GetFormat();
	PSODescriptor.NumRenderTargets = GBufferCount;
	PSODescriptor.DepthStencilFormat = D3D12RHI->GetViewportInfo().DepthStencilFormat; 
	PSODescriptor._4xMsaaState = false; //can't use msaa in deferred rendering.

	// If don't find this PSO, create new PSO and PrimitiveBatch
	GraphicsPSOManager->CreatePSO(PSODescriptor);

	if (PSOPrimitiveBatchMap.find(PSODescriptor) == PSOPrimitiveBatchMap.end())
	{
		PSOPrimitiveBatchMap.emplace(std::make_pair(PSODescriptor, SPrimitiveBatch()));
		SPrimitiveBatch& PrimitiveBatch = PSOPrimitiveBatchMap[PSODescriptor];

		if (primitiveType == D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT)
		{
			PrimitiveBatch.PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		}
		else if (primitiveType == D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
		{
			PrimitiveBatch.PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		}
		else if (primitiveType == D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
		{
			PrimitiveBatch.PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
		else
		{
			assert(0);
		}
	}

	// Update vertex buffer
	SPrimitiveBatch& PrimitiveBatch = PSOPrimitiveBatchMap[PSODescriptor];
	PrimitiveBatch.CurrentVertexNum = (int)vertices.size();

	if (PrimitiveBatch.CurrentVertexNum > 0)
	{
		const UINT VbByteSize = (UINT)vertices.size() * sizeof(SPrimitiveVertex);
		PrimitiveBatch.VertexBufferRef = D3D12RHI->CreateVertexBuffer(vertices.data(), VbByteSize);
	}
	else
	{
		PrimitiveBatch.VertexBufferRef = nullptr;
	}
}

void CRender::PrimitivesPass()
{
	GatherAllPrimitiveBatches();

	// Transit to render target state
	D3D12RHI->TransitionResource(GBufferBaseColor->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12RHI->TransitionResource(GBufferNormal->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12RHI->TransitionResource(GBufferWorldPos->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12RHI->TransitionResource(GBufferORM->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12RHI->TransitionResource(GBufferVelocity->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	D3D12RHI->TransitionResource(GBufferEmissive->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Specify the renderTargets we are going to render to.
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RtvDescriptors;
	RtvDescriptors.push_back(GBufferBaseColor->GetRTV()->GetDescriptorHandle());
	RtvDescriptors.push_back(GBufferNormal->GetRTV()->GetDescriptorHandle());
	RtvDescriptors.push_back(GBufferWorldPos->GetRTV()->GetDescriptorHandle());
	RtvDescriptors.push_back(GBufferORM->GetRTV()->GetDescriptorHandle());
	RtvDescriptors.push_back(GBufferVelocity->GetRTV()->GetDescriptorHandle());
	RtvDescriptors.push_back(GBufferEmissive->GetRTV()->GetDescriptorHandle());

	auto DescriptorCache = D3D12RHI->GetDevice()->GetCommandContext()->GetDescriptorCache();
	CD3DX12_GPU_DESCRIPTOR_HANDLE GpuHandle;
	CD3DX12_CPU_DESCRIPTOR_HANDLE CpuHandle;
	DescriptorCache->AppendRtvDescriptors(RtvDescriptors, GpuHandle, CpuHandle);

	CommandList->OMSetRenderTargets(GBufferCount, &CpuHandle, true, &DepthStencilView());

	// Draw all PrimitiveBatchs
	for (const auto& Pair : PSOPrimitiveBatchMap)
	{
		const SGraphicsPSODescriptor& PSODescriptor = Pair.first;
		const SPrimitiveBatch& PrimitiveBatch = Pair.second;

		if (PrimitiveBatch.CurrentVertexNum > 0)
		{
			// Set PSO	
			CommandList->SetPipelineState(GraphicsPSOManager->GetPSO(PSODescriptor));

			// Set RootSignature
			CShader* Shader = PSODescriptor.Shader;
			CommandList->SetGraphicsRootSignature(Shader->RootSignature.Get()); //should before binding

			// Set paramters
			Shader->SetParameter("cbPass", BasePassCBRef);

			// Bind paramters
			Shader->BindParameters();

			// Set vertex buffer
			D3D12RHI->SetVertexBuffer(PrimitiveBatch.VertexBufferRef, 0, sizeof(SPrimitiveVertex), PrimitiveBatch.CurrentVertexNum * sizeof(SPrimitiveVertex));

			CommandList->IASetPrimitiveTopology(PrimitiveBatch.PrimitiveType);

			// Draw 
			CommandList->DrawInstanced(PrimitiveBatch.CurrentVertexNum, 1, 0, 0);
		}
	}

	// Transit to generic read state
	D3D12RHI->TransitionResource(GBufferBaseColor->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	D3D12RHI->TransitionResource(GBufferNormal->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	D3D12RHI->TransitionResource(GBufferWorldPos->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	D3D12RHI->TransitionResource(GBufferORM->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	D3D12RHI->TransitionResource(GBufferVelocity->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
	D3D12RHI->TransitionResource(GBufferEmissive->GetTexture()->GetResource(), D3D12_RESOURCE_STATE_GENERIC_READ);
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
