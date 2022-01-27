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

enum Visiblity
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
    Visiblity _tag;
    BvhNode *_leftChild;
    BvhNode *_rightChild;
    BvhNode *_parent;
    const BoundingBox *_boundingBox;

public:
    BvhNode(const BoundingBox *boundingBox);
    inline const int getId() const {return id;};
    inline const BvhNode& getLeftChild() const
    {
        return *_leftChild;
    }

    inline const BvhNode getRightChild() const
    {
        return *_rightChild;
    }

    inline const BvhNode& getParent() const
    {
        return *_parent;
    }

    const BvhNode& sibling() const; 
    
    const BvhNode& getChild(const NodeType &type) const;

    inline void setType(const NodeType &type)
    {
        _type = type;
    }
    
    bool isRoot();
    
    inline const BoundingBox& getBoundingBox() const
    {
        return *_boundingBox;
    }

    static BvhNode merge(BvhNode &left, BvhNode &right);
};
#endif