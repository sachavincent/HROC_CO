#ifndef BVHNODE_HPP
#define BVHNODE_HPP
#include <stdlib.h>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

class BoundingBox;
class Object;

enum NodeType
{
    LEFT,
    RIGHT
};

enum Visibility
{
    null,
    VISIBLE,
    POTENTIAL_OCCLUDERS,
    UNKNOWN
};
class BvhNode
{
private:
    int id;
    NodeType type;
    Visibility tag;
    std::shared_ptr<BvhNode>leftChild;
    std::shared_ptr<BvhNode>rightChild;
    std::shared_ptr<BvhNode> parent;
    std::shared_ptr<BoundingBox> boundingBox;

public:
    BvhNode(std::shared_ptr<BoundingBox> _boundingBox, int _id);

    ~BvhNode();

    inline const Visibility getVisibility() const { return tag; }

    inline const int getId() const { return id; }

    inline const bool hasLeftChild() const
    {
        return leftChild != nullptr;
    }

    inline const bool hasRightChild() const
    {
        return rightChild != nullptr;
    }

    inline std::shared_ptr<BvhNode> getLeftChild() const
    {
        return leftChild;
    }

    inline std::shared_ptr<BvhNode> getRightChild() const
    {
        return rightChild;
    }

    inline std::shared_ptr<BvhNode> getParent() const
    {
        return parent;
    }

    inline const NodeType &getType() const
    {
        return type;
    }

    std::shared_ptr<BvhNode> sibling() const;

    std::shared_ptr<BvhNode> getChild(const NodeType &_type) const;

    inline void setType(const NodeType &_type)
    {
        type = _type;
    }

    inline void setVisibility(const Visibility &_tag)
    {
        tag = _tag;
    }

    bool isRoot();

    inline std::shared_ptr<BoundingBox> getBoundingBox()
    {
        return boundingBox;
    }

    inline bool isLeaf() {return !hasLeftChild() && ! hasRightChild();}

    std::vector<std::shared_ptr<Object>> getObjectsInLeafs();
    
    static std::shared_ptr<BvhNode>merge(std::shared_ptr<BvhNode>_left, std::shared_ptr<BvhNode>_right, int _newId);
};
#endif