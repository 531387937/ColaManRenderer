#pragma once
#include <memory>

#include "BoundingBox.h"
#include "Component/MeshComponent.h"

struct SBVHPrimitiveInfo
{
public:
    SBVHPrimitiveInfo(size_t primitiveIdx, CBoundingBox bounds):PrimitiveIdx(primitiveIdx),Bounds(bounds),Centroid(bounds.GetCenter()){}
public:
    size_t PrimitiveIdx = 0;

    CBoundingBox Bounds;

    SVector3 Centroid = SVector3::Zero;
};

struct SBVHBuildNode
{
    void InitLeaf(int first,int count,const CBoundingBox& bounds)
    {
        Bounds = bounds;
        FirstPrimOffset = first;
        PrimitiveCount = count;
    }

    void InitInterior(int axis,std::unique_ptr<SBVHBuildNode>& left, std::unique_ptr<SBVHBuildNode>& right)
    {
        Bounds = CBoundingBox::Union(left->Bounds,right->Bounds);
        SplitAxis = axis;
        LeftChild = std::move(left);
        RightChild = std::move(right);
    }

    bool IsLeafNode(){return PrimitiveCount>0;}
public:
    CBoundingBox Bounds;

    int SplitAxis = -1;

    std::unique_ptr<SBVHBuildNode> LeftChild = nullptr;

    std::unique_ptr<SBVHBuildNode> RightChild = nullptr;

    int FirstPrimOffset = -1;

    int PrimitiveCount = -1;
};

struct SBVHBucketInfo
{
    int Count = 0;

    CBoundingBox Bounds;
};

struct SBVHLinearNode
{
    bool IsLeafNode() { return PrimitiveCount>0;}
public:
    CBoundingBox Bounds;

    int SplitAxis = -1;

    int SecondChildOffset = -1;

    int FirstPrimOffset = -1;

    int PrimitiveCount = -1;
};

class CWorld;

class CBVHAccelerator
{
public:
    enum class EsplitMethod
    {
        Middle,
        EqualCounts,
        SAH
    };

    CBVHAccelerator(const std::vector<CMeshComponent*>& MeshComponents);

    void DebugBVHTree(CWorld* world);

    void DebugFlattenBVH(CWorld* world);

private:
    std::unique_ptr<SBVHBuildNode> RecursiveBuild(std::vector<SBVHPrimitiveInfo>& primitiveInfoList,int _start,int _end,int& outTotalNodes,
        std::vector<CMeshComponent*> orderedPrimitives);

    int PartitionMiddleMethod(const CBoundingBox& centroidBounds,int splitAxis,std::vector<SBVHPrimitiveInfo>& primitiveInfoList,
        int _start,int _end);

    int PartitionEqualCountsMethod(const CBoundingBox& centroidBounds,int splitAxis,std::vector<SBVHPrimitiveInfo>& primitiveInfoList,
        int _start,int _end);

    int PartitionSAHMethod(const CBoundingBox& bounds,const CBoundingBox& centroidBounds,int splitAxis,std::vector<SBVHPrimitiveInfo>& primitiveInfoList,
        int _start,int _end,float& outCost);

    void CreateLeafNode(std::unique_ptr<SBVHBuildNode>& outLeftNode,const CBoundingBox& Bounds, std::vector<SBVHPrimitiveInfo>& primitiveInfoList,
        int _start, int _end, std::vector<CMeshComponent*>& orderedPrimitives);

    int FlattenBVHTree(std::unique_ptr<SBVHBuildNode>& node,int& offset);

    SColor MapDepthToColor(int depth);

    void DebugBuildNode(CWorld* world,SBVHBuildNode* node, int depth);

private:
    EsplitMethod SplitMethod = EsplitMethod::SAH;

    bool bTryAllAxisForSAH = true;

    const int MaxPrimsInNode = 1;

    std::vector<CMeshComponent*> CachePrimitives;

    std::unique_ptr<SBVHBuildNode> RootNode = nullptr;

    std::vector<SBVHLinearNode> LinearNodes;
};