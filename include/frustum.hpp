#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "camera.hpp"
#include "bvh/boundingbox.hpp"

struct Plan
{
    glm::vec3 normal = {0.f, 1.f, 0.f}; // unit vector
    float distance = 0.f;               // Distance with origin

    Plan() = default;

    Plan(const glm::vec3 &p1, const glm::vec3 &norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, p1))
    {
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

    void update(Camera &camera)
    {
        float fov = camera.getFov();
        float aspect = (double)camera.getResWidth() / camera.getResHeight();
        float halfVSide = camera.getFarDistance() * tanf(fov * .5f);
        float halfHSide = halfVSide * aspect;
        glm::vec3 frontMultFar = camera.getFarDistance() * camera.getDirection();

        nearFace = {camera.getPosition() + camera.getNearDistance() * camera.getDirection(), camera.getDirection()};
        farFace = {camera.getPosition() + frontMultFar, -camera.getDirection()};
        rightFace = {camera.getPosition(),
                     glm::cross(camera.getUpVector(), frontMultFar + camera.getRightVector() * halfHSide)};
        leftFace = {camera.getPosition(),
                    glm::cross(frontMultFar - camera.getRightVector() * halfHSide, camera.getUpVector())};
        topFace = {camera.getPosition(),
                   glm::cross(camera.getRightVector(), frontMultFar - camera.getUpVector() * halfVSide)};
        bottomFace = {camera.getPosition(),
                      glm::cross(frontMultFar + camera.getUpVector() * halfVSide, camera.getRightVector())};
    }




    bool isInFrustum(BoundingBox* bb)
    {
        return bb->isOnOrForwardPlan(topFace) 
            && bb->isOnOrForwardPlan(bottomFace)
            && bb->isOnOrForwardPlan(rightFace)
            && bb->isOnOrForwardPlan(leftFace)
            && bb->isOnOrForwardPlan(farFace)
            && bb->isOnOrForwardPlan(nearFace);

    };



    Plan topFace;
    Plan bottomFace;

    Plan rightFace;
    Plan leftFace;

    Plan farFace;
    Plan nearFace;
};
#endif