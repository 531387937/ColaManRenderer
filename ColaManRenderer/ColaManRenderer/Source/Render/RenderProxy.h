#pragma once

#include <string>
#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "D3D12/D3D12Resource.h"
#include "D3D12/D3D12View.h"
#include "Math/Math.h"
#include "Mesh/BoundingBox.h"

struct SMaterialConstants
{
public:
    SVector4 DiffuseAlbedo;
    SVector3 FresnelR0;
    float Roughness;

    // Used in texture mapping.
    SMatrix MatTransform = SMatrix::Identity;

    SVector3 EmissiveColor;
    UINT ShadingModel;
};

// Defines a subrange of geometry in a TMeshProxy.  This is for when multiple
// geometries are stored in one vertex and index buffer.  It provides the offsets
// and data needed to draw a subset of geometry stores in the vertex and index 
// buffers
struct SSubmeshProxy
{
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    INT BaseVertexLocation = 0;

    // Bounding box of the geometry defined by this submesh. 
    DirectX::BoundingBox Bounds;
};

struct SMeshProxy
{
    // Give it a name so we can look it up by name.
    std::string Name;

    CD3D12VertexBufferRef VertexBufferRef;
    CD3D12IndexBufferRef IndexBufferRef;


    // Data about the buffers.
    UINT VertexByteStride = 0;
    UINT VertexBufferByteSize = 0;
    DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
    UINT IndexBufferByteSize = 0;

    // A MeshGeometry may store multiple geometries in one vertex/index buffer.
    // Use this container to define the Submesh geometries so we can draw
    // the Submeshes individually.
    std::unordered_map<std::string, SSubmeshProxy> SubMeshs;
};

struct SLightShaderParameters
{
    SVector3 Color; // All light
    float Intensity; // All light
    SVector3 Position; // Point/Spot light only
    float Range; // Point/Spot light only
    SVector3 Direction; // Directional/Spot light only
    float SpotRadius; // Spot light only
    SVector2 SpotAngles; // Spot light only
    UINT LightType;
    INT ShadowMapIdx = 0;

    SMatrix LightProj = SMatrix::Identity;
    SMatrix ShadowTransform = SMatrix::Identity;

    // AreaLight
    SVector3 AreaLightPoint0InWorld;
    float LightPad2;
    SVector3 AreaLightPoint1InWorld;
    float LightPad3;
    SVector3 AreaLightPoint2InWorld;
    float LightPad4;
    SVector3 AreaLightPoint3InWorld;
    float LightPad5;
};

struct SLightCommonData
{
    UINT LightCount = 0;
};

#define MAX_LIGHT_COUNT_IN_TILE 500

struct STileLightInfo
{
    UINT LightIndices[MAX_LIGHT_COUNT_IN_TILE];
    UINT LightCount;
};

struct SPassConstants
{
    SMatrix View = SMatrix::Identity;
    SMatrix InvView = SMatrix::Identity;
    SMatrix Proj = SMatrix::Identity;
    SMatrix InvProj = SMatrix::Identity;
    SMatrix ViewProj = SMatrix::Identity;
    SMatrix InvViewProj = SMatrix::Identity;
    SMatrix PrevViewProj = SMatrix::Identity;
    SVector3 EyePosW = {0.0f, 0.0f, 0.0f};
    float cbPassPad1 = 0.0f;
    SVector2 RenderTargetSize = {0.0f, 0.0f};
    SVector2 InvRenderTargetSize = {0.0f, 0.0f};
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;

    SVector4 FogColor = {0.7f, 0.7f, 0.7f, 1.0f};
    float gFogStart = 5.0f;
    float gFogRange = 150.0f;
    SVector2 cbPassPad2;
};

struct SObjectConstants
{
    SMatrix World = SMatrix::Identity;
    SMatrix PrevWorld = SMatrix::Identity;
    SMatrix TexTransform = SMatrix::Identity;
};

struct SSpritePassConstants
{
    SMatrix ScreenToNDC = SMatrix::Identity;
};

struct SSpriteObjectConstants
{
    UINT TexRegisterOffset;
    UINT ObjPad0;
    UINT ObjPad1;
    UINT ObjPad2;
};

struct SBlurSettingsConstants
{
    INT gBlurRadius;

    // Support up to 11 blur weights.
    float w0;
    float w1;
    float w2;

    float w3;
    float w4;
    float w5;
    float w6;

    float w7;
    float w8;
    float w9;
    float w10;
};

struct SObjectSDFDescriptor
{
    SMatrix ObjWorld;
    SMatrix ObjInvWorld;
    SMatrix ObjInvWorld_IT;

    int SDFIndex;
    int pad1;
    int pad2;
    int pad3;
};

struct SSDFConstants
{
    UINT ObjectCount;
    UINT Pad1;
    UINT Pad2;
    UINT Pad3;
};

struct SSSAOPassConstants
{
    SMatrix ProjTex;

    // Coordinates given in view space.
    float OcclusionRadius;
    float OcclusionFadeStart;
    float OcclusionFadeEnd;
    float SurfaceEpsilon;
};

struct SDeferredLightingPassConstants
{
    UINT EnableSSAO;
};

struct SPrefilterEnvironmentConstant
{
    SMatrix View = SMatrix::Identity;
    SMatrix Proj = SMatrix::Identity;
    float Roughness;
};
