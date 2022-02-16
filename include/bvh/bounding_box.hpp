#ifndef BVH_BOUNDING_BOX_HPP
#define BVH_BOUNDING_BOX_HPP

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "frustum.hpp"
#include "object.hpp"
#include "boundingBoxObject.hpp"

/// A bounding box, represented with two extreme points.
struct BoundingBox
{
    glm::vec3 minV, maxV;
    std::shared_ptr<BoundingBoxObject> wireframe;

    BoundingBox() = default;
    BoundingBox(const Object &_o);
    BoundingBox(const glm::vec3 &v);
    BoundingBox(const glm::vec3 &_min, const glm::vec3 &_max);

    virtual BoundingBox merge(const BoundingBox &_A)
    {
        return _A;
    }

    inline BoundingBox &shrink(const BoundingBox &bbox)
    {
        minV = (glm::max)(minV, bbox.minV);
        maxV = (glm::min)(maxV, bbox.maxV);
        return *this;
    }

    inline BoundingBox extend(const BoundingBox &bbox)
    {
        // minV = (glm::min)(minV, bbox.minV);
        // maxV = (glm::max)(maxV, bbox.maxV);
        // wireframe = new AxisBoundingBox(minV, maxV);

        glm::vec3 _center = center();
        glm::vec3 _centerOther = bbox.center();

        glm::vec3 _size = diagonal();
        glm::vec3 _sizeOther = bbox.diagonal();

        float minX = _center[0] - _size[0] / 2;
        float maxX = _center[0] + _size[0] / 2;
        float minY = _center[1] - _size[1] / 2;
        float maxY = _center[1] + _size[1] / 2;
        float minZ = _center[2] - _size[2] / 2;
        float maxZ = _center[2] + _size[2] / 2;

        float minXOther = _centerOther[0] - _sizeOther[0] / 2;
        float maxXOther = _centerOther[0] + _sizeOther[0] / 2;
        float minYOther = _centerOther[1] - _sizeOther[1] / 2;
        float maxYOther = _centerOther[1] + _sizeOther[1] / 2;
        float minZOther = _centerOther[2] - _sizeOther[2] / 2;
        float maxZOther = _centerOther[2] + _sizeOther[2] / 2;

        float newMinX = minX < minXOther ? minX : minXOther;
        float newMaxX = maxX > maxXOther ? maxX : maxXOther;
        float newMinY = minY < minYOther ? minY : minYOther;
        float newMaxY = maxY > maxYOther ? maxY : maxYOther;
        float newMinZ = minZ < minZOther ? minZ : minZOther;
        float newMaxZ = maxZ > maxZOther ? maxZ : maxZOther;

        minV = {newMinX, newMinY, newMinZ};
        maxV = {newMaxX, newMaxY, newMaxZ};
        return *this;
    }
    /*
        inline BoundingBox &extend(const glm::vec3 &v)
        {
            minV = (glm::min)(minV, v);
            maxV = (glm::max)(maxV, v);
            return *this;
        }*/

    inline glm::vec3 diagonal() const
    {
        return maxV - minV;
    }

    inline glm::vec3 center() const
    {
        return (maxV + minV) * float(0.5);
    }

    inline float half_area() const
    {
        auto d = diagonal();
        return (d[0] + d[1]) * d[2] + d[0] * d[1];
    }

    inline float volume() const
    {
        auto d = diagonal();
        return d[0] * d[1] * d[2];
    }

    inline size_t largest_axis() const
    {
        auto d = diagonal();
        size_t axis = 0;
        if (d[0] < d[1])
            axis = 1;
        if (d[axis] < d[2])
            axis = 2;
        return axis;
    }

    inline float largest_extent() const
    {
        return diagonal()[largest_axis()];
    }

    inline bool is_contained_in(const BoundingBox &other) const
    {
        return maxV[0] <= other.maxV[0] && minV[0] >= other.minV[0] &&
               maxV[1] <= other.maxV[1] && minV[1] >= other.minV[1] &&
               maxV[2] <= other.maxV[2] && minV[2] >= other.minV[2];
    }

    bool BoundingBox::isOnOrForwardPlan(const Plan &plan) const
    {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        glm::vec3 size = diagonal();
        const float r = size.x * std::abs(plan.normal.x) +
                        size.y * std::abs(plan.normal.y) + size.z * std::abs(plan.normal.z);

        return -r <= plan.getSignedDistanceToPlan(center());
    }

    //! Get a model of the BoundingBox object for debug mode rendering
    inline std::shared_ptr<BoundingBoxObject> getWireframe() { return wireframe; }
};

struct OrientedBoundingBox : public BoundingBox
{
private:
    glm::mat3 transform;

public:
    OrientedBoundingBox() = default;
    OrientedBoundingBox(const Object &_o, glm::mat3 &_transform) : BoundingBox(_o), transform(_transform) {}

    OrientedBoundingBox(const glm::vec3 &v) : BoundingBox(v) {}

    OrientedBoundingBox(const glm::vec3 &_min, const glm::vec3 &_max) : BoundingBox(_min, _max) {}

    ~OrientedBoundingBox()
    {
    }

    inline const glm::mat3 &getTransform() const { return transform; }

    BoundingBox merge(const BoundingBox &other);
};

struct AxisBoundingBox : public OrientedBoundingBox
{
private:
    static glm::mat3 DEFAULT_TRANSFORM;

public:
    AxisBoundingBox() = default;
    AxisBoundingBox(const Object &_o) : OrientedBoundingBox(_o, DEFAULT_TRANSFORM) {}

    AxisBoundingBox(const glm::vec3 &v) : OrientedBoundingBox(v) {}

    AxisBoundingBox(const glm::vec3 &_min, const glm::vec3 &_max) : OrientedBoundingBox(_min, _max) {}

    BoundingBox merge(const BoundingBox &_other);

    ~AxisBoundingBox()
    {
    }

    inline static AxisBoundingBox full()
    {
        return AxisBoundingBox(glm::vec3(-FLT_MAX), glm::vec3(FLT_MAX));
    }

    inline static AxisBoundingBox empty()
    {
        return AxisBoundingBox(glm::vec3(0), glm::vec3(0));
    }
};

#endif
