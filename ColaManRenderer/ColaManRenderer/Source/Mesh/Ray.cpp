#include "Ray.h"

CRay::CRay():MaxDist(CMath::Infinity)
{
    
}

CRay::CRay(const SVector3& origin, const SVector3 direction, float maxDist):Origin(origin),Direction(direction),MaxDist(maxDist)
{
}
