#pragma once

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#define TAA_SAMPLE_COUNT 8
#include <unordered_map>

#include "InputLayout.h"
#include "MeshBatch.h"
#include "PrimitiveBatch.h"
#include "PSO.h"
#include "RenderProxy.h"
#include "ShadowMap.h"
#include "Shader/Shader.h"
#include "World/World.h"

enum class ERenderPass
{
    SHADOWPASS,
    BASEPASS,
    PRIMITIVEPASS,
    SPRITEPASS,
    IRRADIANCE,
    DEFFERREDLIGHTING,
};

struct SRenderSettings
{
    bool bUseTBDR = false;

    bool bEnableTAA = false;

    bool bEnableSSR = false;

    bool bEnableSSAO = true;

    EShadowMapImpl ShadowMapImpl = EShadowMapImpl::PCF;

    bool bDebugSDFScene = false;

    bool bDrawDebugText = false;
};

class CRender
{
public:
    CRender();

    ~CRender();

    bool IsInitialize();

    bool Initialize(int WindowWidth, int WindowHeight, CD3D12RHI* d3d12RHI, CWorld* world,
                    const SRenderSettings& settings);

    void OnResize(int Width, int Height);

    void Draw(const GameTimer& gt);

    void EndFrame();

    void OnDestroy();

private:
    float AspectRatio() const;

    CD3D12Resource* CurrentBackBuffer() const;

    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;

    float* CurrentBackBufferClearColor() const;

    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

private:
    void CreateRenderResource();

    void CreateNullDescriptors();

    void CreateTextures();

    void CreateSceneCaptureCube();

    void CreateGBuffers();

    void CreateSSAOBuffer();

    void CreateBackDepth();

    void CreateColorTextures();

    void CreateMeshProxys();

    void CreateInputLayouts();

    void CreateGlobalShaders();

    void CreateGlobalPSO();

    void CreateComputePSO();

    void CreateComputeShaderResource();

    void SetDescriptorHeaps();

    void GetSkyInfo();

    void UpdateIBLEnvironmentPassCB();

    void CreateIBLEnvironmentMap();

    void UpdateIBLIrradiancePassCB();

    void CreateIBLIrradianceMap();

    void UpdateIBLPrefilterEnvCB();

    void CreateIBLPrefilterEnvMap();

    void GatherAllMeshBatches();

    SMatrix TextureTransform();

    void UpdateLightData();

    void UpdateShadowPassCB(const SSceneView& sceneView, UINT shadowWidth, UINT ShadowHeight);

    void GetShadowPassMeshCommandMap(EShadowMapType Type);

    void ShadowPass();

    void GenerateSingleShadowMap(CShadowMap2D* shadowMap);

    void GenerateOmniShadowMap(CShadowMapCube* shadowMap);

    void UpdateBasePassCB();

    void GetBasePassMeshCommandMap();

    void BasePass();

    void GetBackDepthPassMeshCommandMap();

    void BackDepthPass();

    void UpdateSSAOPassCB();

    void SSAOPass();

    void GatherLightDebugPrimitives(std::vector<CLine>& outLines);

    void GatherAllPrimitiveBatches();

    void GatherPrimitiveBatches(const std::vector<SPrimitiveVertex>& vertices,
                                D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType);

    void PrimitivesPass();

    void UpdateDefferredLightingPassCB();

    void DefferredLightingPass();

    void UpdateSSRPassCB();

    void SSRPass();

    void TAAPass();

    void PostProcessPass();

public:
    const SRenderSettings& GetRenderSettings();

    void ToggleTAA();

    void ToggleSSR();

    void ToggleSSAO();

private:
    bool bInitialize = false;

    int WindowWidth;
    int WindowHeight;

    CWorld* World;

    ID3D12Device* D3DDevice;
    ID3D12GraphicsCommandList* CommandList;

    std::unique_ptr<CD3D12ShaderResourceView> Texture2DNullDescriptor = nullptr;
    std::unique_ptr<CD3D12ShaderResourceView> Texture3DNullDescriptor = nullptr;
    std::unique_ptr<CD3D12ShaderResourceView> TextureCubeNullDescriptor = nullptr;
    std::unique_ptr<CD3D12ShaderResourceView> StructuredBufferNullDescriptor = nullptr;

    //Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> FontHeap;

    const int GBufferCount = 6;
    std::unique_ptr<CRenderTarget2D> GBufferBaseColor;
    std::unique_ptr<CRenderTarget2D> GBufferNormal;
    std::unique_ptr<CRenderTarget2D> GBufferWorldPos;
    std::unique_ptr<CRenderTarget2D> GBufferORM; //Occlusion、Roughness、Metallic
    std::unique_ptr<CRenderTarget2D> GBufferVelocity;
    std::unique_ptr<CRenderTarget2D> GBufferEmissive;

    std::unique_ptr<CRenderTarget2D> SSAOBuffer;

    CD3D12TextureRef ColorTexture = nullptr;

    CD3D12TextureRef CacheColorTexture = nullptr;

    CD3D12TextureRef PrevColorTexture = nullptr;

    std::unique_ptr<CRenderTarget2D> BackDepth = nullptr;

    UINT FrameCount = 0;

private:
    std::unordered_map<std::string, SMeshProxy> MeshProxyMap;

    CInputLayoutManager InputLayoutManager;

    CD3D12ConstantBufferRef ShadowPassCBRef = nullptr;

    CD3D12ConstantBufferRef BasePasCBRef = nullptr;

    CD3D12ConstantBufferRef SSAOPassCBRef = nullptr;

    CD3D12ConstantBufferRef IBLEnvironmentPassCBRef[6];

    CD3D12ConstantBufferRef IBLIrradiancePassCBRef[6];

    const static UINT IBLPrefilterMaxMipLevel = 5;

    CD3D12ConstantBufferRef IBLPrefilterEnvPassCBRef[IBLPrefilterMaxMipLevel * 6];

    CD3D12ConstantBufferRef DefferedLightPassCBRef;

    CD3D12TextureRef BlurMap0 = nullptr;

    //Shaders
    std::unique_ptr<CShader> SingleShadowMapShader = nullptr;

    std::unique_ptr<CShader> OmniSHadowMapShader = nullptr;

    std::unique_ptr<CShader> BackDepthShader = nullptr;

    std::unique_ptr<CShader> IBLEnvironmentShader = nullptr;

    std::unique_ptr<CShader> IBLIrradianceShader = nullptr;

    std::unique_ptr<CShader> IBLPrefilterEnvShader = nullptr;

    std::unique_ptr<CShader> DeferredLightingShader = nullptr;

    std::unique_ptr<CShader> PrimitiveShader = nullptr;

    std::unique_ptr<CShader> SSAOShader = nullptr;

    std::unique_ptr<CShader> SSRShader = nullptr;

    std::unique_ptr<CShader> PostProcessShader = nullptr;

    std::unique_ptr<CShader> TAAShader = nullptr;

    std::unique_ptr<CShader> HorBlurShader = nullptr;

    std::unique_ptr<CShader> verBlurShader = nullptr;

    //Graphics PSO
    std::unique_ptr<CGraphicsPSOManager> GraphicsPSOManager;

    SGraphicsPSODescriptor IBLEnvironmentPSODescriptor;

    SGraphicsPSODescriptor IBLIrradiancePSODescriptor;

    SGraphicsPSODescriptor IBLPrefilterEnvPSODescriptor;

    SGraphicsPSODescriptor DeferredLightingPSODescriptor;

    SGraphicsPSODescriptor SSAOPSODescriptor;

    SGraphicsPSODescriptor SSRPSODescriptor;

    SGraphicsPSODescriptor PostProcessPSODescriptor;

    SGraphicsPSODescriptor TAAPSODescriptor;

    //Compute PSO
    std::unique_ptr<CComputePSOManager> ComputePSOManager;

    SComputePSODescriptor horBlurPSODescriptor;

    SComputePSODescriptor VerBlurPSODescriptor;

    //MeshBatch && MeshCommand
    std::vector<SMeshBatch> MeshBatches;

    std::unordered_map<SGraphicsPSODescriptor, CMeshCommandList> ShadowMeshCommandMap;

    std::unordered_map<SGraphicsPSODescriptor, CMeshCommandList> BaseMeshCommandMap;

    std::unordered_map<SGraphicsPSODescriptor, CMeshCommandList> BackDepthCommandMap;

    const int MaxRenderMeshCount = 100;

    std::unordered_map<SGraphicsPSODescriptor, SPrimitiveBatch> PSOPrimitiveBatchMap;

    //Light
    CD3D12StructuredBufferRef LightShaderParametersBuffer = nullptr;

    CD3D12ConstantBufferRef LightCommondDataBuffer = nullptr;

    UINT LightCount = 0;

    //Shadow
    const UINT MAX_SHADOW_MAP_2D_NUM = 10;

    const UINT MAX_SHADOW_MAP_CUBE_NUM = 5;

    const UINT ShadowSize = 1024;

    std::vector<CD3D12ShaderResourceView*> ShadowMapSRVs;

    std::vector<CD3D12ShaderResourceView*> ShadowMapCubeSRVs;

    //Sky
    CMeshComponent* SkyMeshComponent = nullptr;

    std::string SkyCubeTextureName;

    //PBR
    std::unique_ptr<CSceneCaptureCube> IBLEnvironmentMap;

    std::unique_ptr<CSceneCaptureCube> IBLIrradianceMap;

    std::vector<std::unique_ptr<CSceneCaptureCube>> IBLPrefilterEnvMaps;

    bool bEnableFrustumCulling = false;

    CD3D12RHI* D3D12RHI = nullptr;

    SRenderSettings RenderSetting;

    bool bEnableIBLEnvLighting = false;
};
