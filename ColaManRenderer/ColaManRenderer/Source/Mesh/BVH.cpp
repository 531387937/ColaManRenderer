#include "BVH.h"

#include <algorithm>
#include <stack>

CBVHAccelerator::CBVHAccelerator(const std::vector<CMeshComponent*>& MeshComponents)
{
    std::vector<SBVHPrimitiveInfo> primitiveInfoList;
    for(size_t i = 0;i<MeshComponents.size();i++)
    {
        CBoundingBox worldBound;
        if(MeshComponents[i]->GetWorldBoundingBox(worldBound))
        {
            primitiveInfoList.push_back({i,worldBound});

            CachePrimitives.push_back(MeshComponents[i]);
        }
    }

    std::vector<CMeshComponent*> OrderedPrimitives;
    OrderedPrimitives.reserve(primitiveInfoList.size());

    int TotalNodes = 0;
    RootNode = RecursiveBuild(primitiveInfoList,0,(int)primitiveInfoList.size(),TotalNodes,OrderedPrimitives);

    CachePrimitives.swap(OrderedPrimitives);

    LinearNodes.resize(TotalNodes);
    int Offset = 0;
    FlattenBVHTree(RootNode,Offset);
}

void CBVHAccelerator::DebugBVHTree(CWorld* world)
{
    DebugBuildNode(world, RootNode.get(), 0);
}

void CBVHAccelerator::DebugFlattenBVH(CWorld* world)
{
    if (LinearNodes.empty())
    {
        return;
    }

    int CurrentVisitNodeIdx = 0;
    std::stack<int> NodesToVisit;

    while (true)
    {
        SBVHLinearNode& Node = LinearNodes[CurrentVisitNodeIdx];

        if (Node.IsLeafNode()) // Leaf node
            {
            world->DrawBox3D(Node.Bounds.Min, Node.Bounds.Max, SColor::White);

            if (NodesToVisit.empty())
            {
                break;
            }
            else
            {
                CurrentVisitNodeIdx = NodesToVisit.top();
                NodesToVisit.pop();
            }
            }
        else // Interior node
            {
            world->DrawBox3D(Node.Bounds.Min, Node.Bounds.Max, SColor::Red);

            NodesToVisit.push(Node.SecondChildOffset);
            CurrentVisitNodeIdx++;
            }
    }
}

std::unique_ptr<SBVHBuildNode> CBVHAccelerator::RecursiveBuild(std::vector<SBVHPrimitiveInfo>& primitiveInfoList,
    int _start, int _end, int& outTotalNodes, std::vector<CMeshComponent*> orderedPrimitives)
{
    assert(_start<_end);

    std::unique_ptr<SBVHBuildNode> node = std::make_unique<SBVHBuildNode>();

    outTotalNodes++;

    CBoundingBox bounds;
    for(int i = _start;i<_end;i++)
    {
        bounds = CBoundingBox::Union(bounds,primitiveInfoList[i].Bounds);
    }

    int PrimitiveCount = _end-_start;
    if(PrimitiveCount==1)
    {
        CreateLeafNode(node,bounds,primitiveInfoList,_start,_end,orderedPrimitives);

        return node;
    }
    else
    {
        CBoundingBox CentroidBounds;
        for(int i =_start;i<_end;i++)
        {
            CentroidBounds = CBoundingBox::Union(CentroidBounds,primitiveInfoList[i].Bounds);
        }
        int SplitAxis = CentroidBounds.GetWidestAxis();

        if(CentroidBounds.Max[SplitAxis]==CentroidBounds.Min[SplitAxis])
        {
            CreateLeafNode(node,bounds,primitiveInfoList,_start,_end,orderedPrimitives);

            return node;
        }

        int Mid = (_start+_end)/2;
        switch (SplitMethod)
        {
            case EsplitMethod::Middle:
                {
                    Mid = PartitionMiddleMethod(CentroidBounds,SplitAxis,primitiveInfoList,_start,_end);

                    if(Mid==_start||Mid==_end)
                    {
                        Mid = PartitionEqualCountsMethod(CentroidBounds,SplitAxis,primitiveInfoList,_start,_end);
                    }
                    break;
                }
                
            case EsplitMethod::EqualCounts:
                {
                    Mid = PartitionEqualCountsMethod(CentroidBounds,SplitAxis,primitiveInfoList,_start,_end);
                    break;
                }
            case EsplitMethod::SAH:
                {
                    if(PrimitiveCount<=2)
                    {
                        Mid = PartitionEqualCountsMethod(CentroidBounds,SplitAxis,primitiveInfoList,_start,_end);
                    }
                    else
                    {
                        if(bTryAllAxisForSAH)
                        {
                            float MinCost = CMath::Infinity;
                            int BestAxis = -1, BestMid = -1;
                            for (int CurAxis = 0; CurAxis < 3; CurAxis++) // Try all axis, compute their cost
                                {
                                float CurCost = 0.0f;
                                int CurMid = PartitionSAHMethod(bounds, CentroidBounds, CurAxis, primitiveInfoList, _start, _end, CurCost);

                                if (CurMid != -1 && CurCost < MinCost)
                                {
                                    MinCost = CurCost;
                                    BestAxis = CurAxis;
                                    BestMid = CurMid;
                                }
                                }

                            if (BestAxis != -1) // Use the least costly axis we found
                                {
                                SplitAxis = BestAxis;
                                Mid = BestMid;
                                }
                            else
                            {
                                Mid = -1;
                            }
                        }
                        else // Only consider the widest axis
                            {
                            float Cost = 0.0f;
                            Mid = PartitionSAHMethod(bounds, CentroidBounds, SplitAxis, primitiveInfoList, _start, _end, Cost);
                            }


                        if (Mid == -1) // Create leaf node
                            {
                            CreateLeafNode(node, bounds, primitiveInfoList, _start, _end, orderedPrimitives);

                            return node;
                            }

                        if (Mid == _start || Mid == _end) // Partition fail, use EqualCounts as an alternative
                            {
                            Mid = PartitionEqualCountsMethod(CentroidBounds, SplitAxis, primitiveInfoList, _start, _end);
                            }
                    }

                    break;
                }
        default:
            break;
        }

        node->InitInterior(SplitAxis,
            RecursiveBuild(primitiveInfoList, _start, Mid,
                outTotalNodes, orderedPrimitives),
            RecursiveBuild(primitiveInfoList, Mid, _end,
                outTotalNodes, orderedPrimitives));

        return node;
    }
}

int CBVHAccelerator::PartitionMiddleMethod(const CBoundingBox& centroidBounds, int splitAxis,
    std::vector<SBVHPrimitiveInfo>& primitiveInfoList, int _start, int _end)
{
    float AxisMid = (centroidBounds.Min[splitAxis] + centroidBounds.Max[splitAxis]) / 2;

    SBVHPrimitiveInfo* MidPtr = std::partition(
        &primitiveInfoList[_start], &primitiveInfoList[_end - 1] + 1,
        [splitAxis, AxisMid](const SBVHPrimitiveInfo& PrimitiveInfo)
        {
            return PrimitiveInfo.Centroid[splitAxis] < AxisMid;
        });

    int Mid = int(MidPtr - &primitiveInfoList[0]);

    return Mid;
}

int CBVHAccelerator::PartitionEqualCountsMethod(const CBoundingBox& centroidBounds, int splitAxis,
    std::vector<SBVHPrimitiveInfo>& primitiveInfoList, int _start, int _end)
{
    int Mid = (_start + _end) / 2;
    std::nth_element(&primitiveInfoList[_start], &primitiveInfoList[Mid],
        &primitiveInfoList[_end - 1] + 1,
        [splitAxis](const SBVHPrimitiveInfo& a, const SBVHPrimitiveInfo& b)
        {
            return a.Centroid[splitAxis] < b.Centroid[splitAxis];
        });

    return Mid;
}

int ComputeSAHBucketIndex(int BucketCount, const CBoundingBox& CentroidBounds, int SplitAxis, const SBVHPrimitiveInfo& PrimitiveInfo)
{
    float AxisWidth = CentroidBounds.Max[SplitAxis] - CentroidBounds.Min[SplitAxis];
    float Offset = PrimitiveInfo.Centroid[SplitAxis] - CentroidBounds.Min[SplitAxis];

    int BucketIdx = int(BucketCount * (Offset / AxisWidth));
    if (BucketIdx == BucketCount)
    {
        BucketIdx = BucketCount - 1;
    }
    assert(BucketIdx >= 0);
    assert(BucketIdx < BucketCount);

    return BucketIdx;
}

int CBVHAccelerator::PartitionSAHMethod(const CBoundingBox& bounds, const CBoundingBox& centroidBounds, int splitAxis,
    std::vector<SBVHPrimitiveInfo>& primitiveInfoList, int _start, int _end, float& outCost)
{
    const int BucketCount = 12;
    SBVHBucketInfo Buckets[BucketCount];

    // Initialize BucketInfos for SAH partition buckets
    for (int i = _start; i < _end; ++i)
    {
        int BucketIdx = ComputeSAHBucketIndex(BucketCount, centroidBounds, splitAxis, primitiveInfoList[i]);

        Buckets[BucketIdx].Count++;
        Buckets[BucketIdx].Bounds = CBoundingBox::Union(Buckets[BucketIdx].Bounds, primitiveInfoList[i].Bounds);
    }

    // Compute costs for splitting after each bucket
    float Cost[BucketCount - 1];
    for (int i = 0; i < BucketCount - 1; ++i)
    {
        CBoundingBox Box0, Box1;
        int Count0 = 0, Count1 = 0;
        for (int j = 0; j <= i; ++j) 
        {
            Box0 = CBoundingBox::Union(Box0, Buckets[j].Bounds);
            Count0 += Buckets[j].Count;
        }
        for (int j = i + 1; j < BucketCount; ++j)
        {
            Box1 = CBoundingBox::Union(Box1, Buckets[j].Bounds);
            Count1 += Buckets[j].Count;
        }

        Cost[i] = 1.0f + (Count0 * Box0.GetSurfaceArea() + Count1 * Box1.GetSurfaceArea()) / bounds.GetSurfaceArea();
    }

    // Find bucket to split at that minimizes SAH metric
    float MinCost = Cost[0];
    int MinCostSplitBucket = 0;
    for (int i = 1; i < BucketCount - 1; ++i)
    {
        if (Cost[i] < MinCost) 
        {
            MinCost = Cost[i];
            MinCostSplitBucket = i;
        }
    }

    outCost = MinCost;

    // Either split primitives at selected SAH bucket or create leaf
    int PrimitiveCount = _end - _start;
    float LeafCost = float(PrimitiveCount);
    if (PrimitiveCount > MaxPrimsInNode || MinCost < LeafCost)
    {
        SBVHPrimitiveInfo* MidPtr = std::partition(
            &primitiveInfoList[_start], &primitiveInfoList[_end - 1] + 1,
            [=](const SBVHPrimitiveInfo& PrimitiveInfo)
            {
                int BucketIdx = ComputeSAHBucketIndex(BucketCount, centroidBounds, splitAxis, PrimitiveInfo);

                return BucketIdx <= MinCostSplitBucket;
            });

        int Mid = int(MidPtr - &primitiveInfoList[0]);
        return Mid;
    }
    else 
    {
        // Create leaf node

        return -1;
    }
}

void CBVHAccelerator::CreateLeafNode(std::unique_ptr<SBVHBuildNode>& outLeftNode, const CBoundingBox& Bounds,
    std::vector<SBVHPrimitiveInfo>& primitiveInfoList, int _start, int _end,
    std::vector<CMeshComponent*>& orderedPrimitives)
{
    int FirstPrimOffset = (int)orderedPrimitives.size();
    for (int i = _start; i < _end; ++i)
    {
        size_t PrimitiveIdx = primitiveInfoList[i].PrimitiveIdx;
        orderedPrimitives.push_back(CachePrimitives[PrimitiveIdx]);
    }

    int PrimitiveCount = _end - _start;
    outLeftNode->InitLeaf(FirstPrimOffset, PrimitiveCount, Bounds);
}

int CBVHAccelerator::FlattenBVHTree(std::unique_ptr<SBVHBuildNode>& node, int& offset)
{
    SBVHLinearNode& LinearNode = LinearNodes[offset];
    int MyOffset = offset;
    offset++;

    LinearNode.Bounds = node->Bounds;
    if (node->IsLeafNode()) // Left node
        {
        assert(node->LeftChild == nullptr);
        assert(node->RightChild == nullptr);

        LinearNode.FirstPrimOffset = node->FirstPrimOffset;
        LinearNode.PrimitiveCount = node->PrimitiveCount;
        }
    else // Interior node
        {
        LinearNode.SplitAxis = node->SplitAxis;

        FlattenBVHTree(node->LeftChild, offset);

        LinearNode.SecondChildOffset = FlattenBVHTree(node->RightChild, offset);
        }

    return MyOffset;
}

SColor CBVHAccelerator::MapDepthToColor(int depth)
{
    SColor Color;
    switch (depth)
    {
    case 0:
        Color = SColor::Red;
        break;
    case 1:
        Color = SColor::Yellow;
        break;
    case 2:
        Color = SColor::Green;
        break;
    case 3:
        Color = SColor::Blue;
        break;
    default:
        Color = SColor::White;
        break;
    }

    return Color;
}

void CBVHAccelerator::DebugBuildNode(CWorld* world, SBVHBuildNode* node, int depth)
{
    if (node == nullptr)
    {
        return;
    }

    SColor Color = MapDepthToColor(depth);

    SVector3 Offset = SVector3(0.1f) * float(std::clamp(5 - depth, 0, 5));

    world->DrawBox3D(node->Bounds.Min - Offset, node->Bounds.Max + Offset, Color);

    DebugBuildNode(world, node->LeftChild.get(), depth + 1);
    DebugBuildNode(world, node->RightChild.get(), depth + 1);
}
