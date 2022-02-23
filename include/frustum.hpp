#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "camera.hpp"
#include "bvh/boundingbox.hpp"
#include "bvh/bvhnode.hpp"
#include "object.hpp"
#include <array>
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

    Plan(const glm::vec3 &p1,const glm::vec3 &p2,const glm::vec3 &p3){
        glm::vec3 x = p2-p1;
        glm::vec3 y = p3-p1;
        normal = glm::normalize(glm::cross(x,y));
        distance = glm::dot(normal,p1)/sqrt(normal.x*normal.x + normal.y * normal.y +normal.z*normal.z);
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
        auto proj = camera->getProjectionMatrix();
        auto view = camera->getViewMatrix();
        std::array<glm::vec3, 8> _cameraFrustumCornerVertices{
            {
                {-1.0f, -1.0f, 1.0f},
                {1.0f, -1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f},
                {-1.0f, 1.0f, 1.0f},
                {-1.0f, -1.0f, -1.0f},
                {1.0f, -1.0f, -1.0f},
                {1.0f, 1.0f, -1.0f},
                {-1.0f, 1.0f, -1.0f},
            }};

        const auto inv = glm::inverse(proj * view);
        std::array<glm::vec3, 8> _frustumVertices;

        std::transform(
            _cameraFrustumCornerVertices.begin(),
            _cameraFrustumCornerVertices.end(),
            _frustumVertices.begin(),
            [&](glm::vec3 p)
            {
                auto v = inv * glm::vec4(p, 1.0f);
                return glm::vec3(v) / v.w;
            });
        glm::vec3 *vertices = _frustumVertices.data();
        farFace = Plan(vertices[0],vertices[1],vertices[2]);
        nearFace = Plan(vertices[4],vertices[6],vertices[5]);
        bottomFace = Plan(vertices[0],vertices[4],vertices[5]);
        rightFace = Plan(vertices[1],vertices[5],vertices[6]);
        topFace = Plan(vertices[2],vertices[7],vertices[6]);
        leftFace = Plan(vertices[3],vertices[7],vertices[0]);
    }

    bool isInFrustum(std::shared_ptr<BoundingBox> bb) const
    {
        return bb->isOnOrForwardPlan(farFace)&& bb->isOnOrForwardPlan(nearFace)&& bb->isOnOrForwardPlan(topFace)&& bb->isOnOrForwardPlan(bottomFace)&& bb->isOnOrForwardPlan(leftFace)&& bb->isOnOrForwardPlan(rightFace);
    };

    std::vector<std::shared_ptr<BvhNode>> ViewFrustumCulling(std::vector<std::shared_ptr<BvhNode>> occludeeGroups) const
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