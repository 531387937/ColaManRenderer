#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Vertex.h"
#include "BoundingBox.h"
#include "Texture/Texture.h"

struct SMeshSDFDescriptor
{
    SVector3 Center;
    float Extent;

    int Resolution;
    int pad1;
    int pad2;
    int pad3;
};

class CMesh
{
public:
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;

    CMesh();

    CMesh(CMesh&&) = default;

    CMesh(const CMesh&) = delete;

    CMesh& operator=(const CMesh&) = delete;

public:
    /// Creates a box centered at the origin with the given dimensions, where each
    /// face has m rows and n columns of vertices.
    void CreateBox(float width, float height, float depth, uint32 numSubdivisions);

    /// Creates a sphere centered at the origin with the given radius.  The
    /// slices and stacks parameters control the degree of tessellation.
    void CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);


    /// Creates a cylinder parallel to the y-axis, and centered about the origin.  
    /// The bottom and top radius can vary to form various cone shapes rather than true
    // cylinders.  The slices and stacks parameters control the degree of tessellation.
    void CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);


    /// Creates an mxn grid in the xz-plane with m rows and n columns, centered
    /// at the origin with the specified width and depth.
    void CreateGrid(float width, float depth, uint32 m, uint32 n);

    /// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
    void CreateQuad(float x, float y, float w, float h, float depth);

    const std::vector<uint16>& GetIndices16() const;

    std::string GetInputLayoutName() const;

    void GenerateIndices16();

public:
    void GenerateBoundingBox();

    CBoundingBox GetBoundingBox() { return BoundingBox; }

    // void SetSDFTexture(std::unique_ptr<CTexture3D>& InSDFTexture)
    // { 
    // 	SDFTexture = std::move(InSDFTexture);
    // }

    // CTexture3D* GetSDFTexture()
    // {
    // 	if (SDFTexture)
    // 	{
    // 		return SDFTexture.get();
    // 	}
    // 	else
    // 	{
    // 		return nullptr;
    // 	}
    // }

private:
    void Subdivide();

    SVertex MidPoint(const SVertex& v0, const SVertex& v1);

    void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

    void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount,
                                uint32 stackCount);

public:
    std::string MeshName;

    std::vector<SVertex> Vertices;

    std::vector<uint32> Indices32;

    std::vector<uint16> Indices16;

    std::string InputLayoutName;

    CBoundingBox BoundingBox;

    // SDF
    //std::unique_ptr<CTexture3D> SDFTexture = nullptr;

    SMeshSDFDescriptor SDFDescriptor;
};
