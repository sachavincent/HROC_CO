#include "frustum.hpp"
#include "bvh/bounding_box.hpp"

bool Frustum::isInFrustum(BoundingBox *bb)
{
    return bb->isOnOrForwardPlan(topFace) && bb->isOnOrForwardPlan(bottomFace) && bb->isOnOrForwardPlan(rightFace) && bb->isOnOrForwardPlan(leftFace) && bb->isOnOrForwardPlan(farFace) && bb->isOnOrForwardPlan(nearFace);
}