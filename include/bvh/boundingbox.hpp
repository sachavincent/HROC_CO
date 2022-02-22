#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP
#include <stdlib.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "boundingBoxObject.hpp"

class BvhNode;
struct Plan;

class BoundingBox
{
protected:
    glm::vec3 size;
    glm::vec3 center;
    std::shared_ptr<BoundingBoxObject> wireframe;
    std::shared_ptr<BvhNode> node;
    std::shared_ptr<const Object> object;

public:
#ifdef HROC_TESTS
    BoundingBox() : wireframe(nullptr)
    {
    }
#endif
    BoundingBox(const Object &_o);

    BoundingBox(glm::vec3 _center, glm::vec3 _size);

    virtual ~BoundingBox()
    {
    }

    inline const glm::vec3 &getSize() const { return size; }
    inline const glm::vec3 &getCenter() const { return center; }

    virtual BoundingBox *merge(BoundingBox *_A) = 0;
    virtual bool isOnOrForwardPlan(const Plan &plan) const;

    inline void setNode(std::shared_ptr<BvhNode> _node) { node = _node; }
    inline const std::shared_ptr<BvhNode> &getNode() const { return node; }

    inline const std::shared_ptr<const Object> &getObject() const { return object; }

    static float distance(std::shared_ptr<BoundingBox> _A, std::shared_ptr<BoundingBox> _B);

    //! Get a model of the BoundingBox object for debug mode rendering
    inline std::shared_ptr<BoundingBoxObject> getWireframe() { return wireframe; }
};

class OrientedBoundingBox : public BoundingBox
{
private:
    glm::mat3 transform;

public:
    OrientedBoundingBox(const Object &_o, glm::mat3 &_transform);

    OrientedBoundingBox(glm::vec3 _center, glm::vec3 _size);

    ~OrientedBoundingBox()
    {
    }

    inline const glm::mat3 &getTransform() const { return transform; }

    BoundingBox *merge(BoundingBox *other);
};

class AxisBoundingBox : public OrientedBoundingBox
{
private:
    static glm::mat3 DEFAULT_TRANSFORM;

public:
    AxisBoundingBox(const Object &_o);

    AxisBoundingBox(glm::vec3 _center, glm::vec3 _size);

    BoundingBox *merge(BoundingBox *_other);

    ~AxisBoundingBox()
    {
    }
};
#endif