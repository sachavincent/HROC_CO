#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "camera.hpp"
#include "bvh/boundingbox.hpp"
#include "bvh/bvhnode.hpp"
#include "object.hpp"
#include <vector>

struct Plan
{
public:
    glm::vec3 normal = {0.f, 1.f, 0.f}; // unit vector
    float distance = 0.f;               // Distance with origin
    glm::vec3 pos;

    Plan() = default;

    Plan(const glm::vec3 &p1, const glm::vec3 &norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, p1))
    {
        pos = p1;
    }

    float getSignedDistanceToPlan(const glm::vec3 &point) const
    {
        return glm::dot(normal, point) - distance;
    }
};
struct Frustum
{
public:
    Frustum() {}
    Frustum(Camera *camera) { update(camera); }

    void update(Camera *camera)
    {
        float fov = camera->getFov();
        float aspect = (double)camera->getResWidth() / camera->getResHeight();
        float halfVSide = camera->getFarDistance() * tanf(fov * .5f);
        float halfHSide = halfVSide * aspect;
        glm::vec3 frontMultFar = camera->getFarDistance() * camera->getDirection();

        nearFace = Plan(camera->getPosition() + camera->getNearDistance() * camera->getDirection(),
                        camera->getDirection());
        farFace = Plan(camera->getPosition() + frontMultFar, -camera->getDirection());
        rightFace = Plan(camera->getPosition(),
                         glm::cross(camera->getUpVector(), frontMultFar + camera->getRightVector() * halfHSide));
        leftFace = Plan(camera->getPosition(),
                        glm::cross(frontMultFar - camera->getRightVector() * halfHSide, camera->getUpVector()));
        topFace = Plan(camera->getPosition(),
                       glm::cross(camera->getRightVector(), frontMultFar - camera->getUpVector() * halfVSide));
        bottomFace = Plan(camera->getPosition(),
                          glm::cross(frontMultFar + camera->getUpVector() * halfVSide, camera->getRightVector()));
    }

    bool isInFrustum(std::shared_ptr<BoundingBox> bb) const
    {
        return bb->isOnOrForwardPlan(topFace) && bb->isOnOrForwardPlan(bottomFace) && bb->isOnOrForwardPlan(rightFace) && bb->isOnOrForwardPlan(leftFace) && bb->isOnOrForwardPlan(farFace) && bb->isOnOrForwardPlan(nearFace);
    };

    std::vector<std::shared_ptr<BvhNode>> ViewFrustumCulling(const std::vector<std::shared_ptr<BvhNode>> &occludeeGroups) const
    {
        std::vector<std::shared_ptr<BvhNode>> occ;
        for (auto it = occludeeGroups.begin(); it != occludeeGroups.end(); it++)
        {
            if (isInFrustum((*it)->getBoundingBox()))
            {
                occ.push_back(std::shared_ptr<BvhNode>(*it));
            }
        }
        return occ;
    };

    Plan topFace;
    Plan bottomFace;

    Plan rightFace;
    Plan leftFace;

    Plan farFace;
    Plan nearFace;
};

#endif