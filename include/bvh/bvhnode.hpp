#ifndef BVH_NODE_HPP
#define BVH_NODE_HPP

#include <climits>
#include <memory>
#include <vector>
#include <map>
#include <cassert>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "bvhnode.hpp"
#include "boundingBoxObject.hpp"
#include "bounding_box.hpp"
#include "utilities.hpp"
#include "utils/idgenerator.hpp"

enum Visibility
{
    null,
    VISIBLE,
    POTENTIAL_OCCLUDERS,
    UNKNOWN
};
using IndexType = typename SizedIntegerType<sizeof(float) * CHAR_BIT>::Unsigned;
struct BvhNode : public std::enable_shared_from_this<BvhNode>
{
    size_t id;
    Visibility tag = Visibility::null;
    std::shared_ptr<BoundingBox> boundingBox;
    float bounds[6];
    IndexType primitive_count;
    IndexType first_child_or_primitive;
    IndexType parent;

    BvhNode() : id(IdGenerator::GetUniqueId())
    {
    }
    inline size_t getId() const { return id; }
    bool is_leaf() const { return primitive_count != 0; }

    inline const Visibility getVisibility() const { return tag; }
    inline void setVisibility(const Visibility &_tag)
    {
        tag = _tag;
    }
    inline std::shared_ptr<BoundingBox> getBoundingBox()
    {
        return boundingBox;
    }
    inline bool hasLeftChild(const std::unique_ptr<BvhNode[]> &_nodes)
    {
        if (is_leaf())
            return false;
        return &_nodes[first_child_or_primitive];
    }
    inline bool hasRightChild(const std::unique_ptr<BvhNode[]> &_nodes)
    {
        if (is_leaf())
            return false;
        auto *leftChild = &_nodes[first_child_or_primitive];
        auto *rightChild = leftChild + 1;
        return rightChild;
    }
    inline BvhNode getLeftChild(const std::unique_ptr<BvhNode[]> &_nodes) { return _nodes[first_child_or_primitive]; }
    inline BvhNode getRightChild(const std::unique_ptr<BvhNode[]> &_nodes)
    {
        BvhNode *leftChild = &_nodes[first_child_or_primitive];
        BvhNode *rightChild = leftChild + 1;
        return *rightChild;
    }
    inline void setVisibilityRecursive(const Visibility &_tag, const std::unique_ptr<BvhNode[]> &_nodes)
    {
        tag = _tag;
        auto *leftChild = &_nodes[first_child_or_primitive];
        auto *rightChild = leftChild + 1;
        if (hasLeftChild(_nodes))
            leftChild->setVisibilityRecursive(_tag, _nodes);
        if (hasRightChild(_nodes))
            rightChild->setVisibilityRecursive(_tag, _nodes);
    }
    /// Accessor to simplify the manipulation of the bounding box of a node.
    /// This type is convertible to a `BoundingBox`.
    struct BoundingBoxProxy
    {
        BvhNode &node;

        BoundingBoxProxy(BvhNode &node)
            : node(node)
        {
        }

        BoundingBoxProxy &operator=(const BoundingBox &bbox)
        {
            node.bounds[0] = bbox.minV[0];
            node.bounds[1] = bbox.maxV[0];
            node.bounds[2] = bbox.minV[1];
            node.bounds[3] = bbox.maxV[1];
            node.bounds[4] = bbox.minV[2];
            node.bounds[5] = bbox.maxV[2];
            return *this;
        }

        operator AxisBoundingBox() const
        {
            return AxisBoundingBox(
                glm::vec3(node.bounds[0], node.bounds[2], node.bounds[4]),
                glm::vec3(node.bounds[1], node.bounds[3], node.bounds[5]));
        }

        AxisBoundingBox to_bounding_box() const
        {
            return static_cast<AxisBoundingBox>(*this);
        }

        float half_area() const { return to_bounding_box().half_area(); }

        BoundingBoxProxy &extend(const BoundingBox &bbox)
        {
            return *this = to_bounding_box().extend(bbox);
        }
    };

    BoundingBoxProxy bounding_box_proxy()
    {
        return BoundingBoxProxy(*this);
    }

    const BoundingBoxProxy bounding_box_proxy() const
    {
        return BoundingBoxProxy(*const_cast<BvhNode *>(this));
    }

    void RecursiveAdd(std::vector<std::shared_ptr<BvhNode>> &vec, const std::unique_ptr<BvhNode[]> &_nodes)
    {
        auto *leftChild = &_nodes[first_child_or_primitive];
        auto *rightChild = leftChild + 1;
        if (tag == Visibility::UNKNOWN)
        {
            vec.push_back(shared_from_this());
        }
        if (hasLeftChild(_nodes))
        {
            leftChild->RecursiveAdd(vec, _nodes);
        }
        if (hasRightChild(_nodes))
        {
            rightChild->RecursiveAdd(vec, _nodes);
        }
    }
};
#endif