#ifndef BVHNODE_HPP
#define BVHNODE_HPP
#include <stdlib.h>
#include <iostream>
#include <map>
#include "boundingbox.hpp"
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
    NodeType _type;
    Visibility _tag;
    BvhNode *_leftChild;
    BvhNode *_rightChild;
    BvhNode *_parent;
    const BoundingBox *_boundingBox;

public:
    BvhNode(const BoundingBox *boundingBox,int _id);

    inline const Visibility getVisibility() const { return _tag; }

    inline const int getId() const { return id; }

    inline const bool hasLeftChild() const
    {
        return _leftChild != nullptr;
    }

    inline const bool hasRightChild() const
    {
        return _rightChild != nullptr;
    }

    inline BvhNode *getLeftChild() const
    {
        return _leftChild;
    }

    inline BvhNode *getRightChild() const
    {
        return _rightChild;
    }

    inline BvhNode *getParent() const
    {
        return _parent;
    }
    
    inline const NodeType &getType() const
    {
        return _type;
    }

    BvhNode *sibling() const;

    BvhNode *getChild(const NodeType &type) const;

    inline void setType(const NodeType &type)
    {
        _type = type;
    }
    inline void setVisibility(const Visibility &tag)
    {
        _tag = tag;
    }

    bool isRoot();

    inline const BoundingBox &getBoundingBox() const
    {
        return *_boundingBox;
    }

    static BvhNode* merge(BvhNode *left, BvhNode *right,int newid);
};
#endif