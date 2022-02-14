#ifndef BVHNODE_HPP
#define BVHNODE_HPP
#include <stdlib.h>
#include <iostream>
#include <map>
#include <memory>

class BoundingBox;

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
    BvhNode *leftChild;
    BvhNode *rightChild;
    BvhNode *parent;
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

    inline BvhNode *getLeftChild() const
    {
        return leftChild;
    }

    inline BvhNode *getRightChild() const
    {
        return rightChild;
    }

    inline BvhNode *getParent() const
    {
        return parent;
    }

    inline const NodeType &getType() const
    {
        return type;
    }

    BvhNode *sibling() const;

    BvhNode *getChild(const NodeType &_type) const;

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

    static BvhNode *merge(BvhNode *_left, BvhNode *_right, int _newId);
};
#endif