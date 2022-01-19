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
    NodeType type;
    Visiblity tag;
    BvhNode *leftChild;
    BvhNode *rightChild;
    BvhNode *parent;
    BoundingBox *boundingBox;

public:
    BvhNode(BoundingBox &bb);
    BvhNode &getLeftChild();
    BvhNode &getRightChild();
    BvhNode &sibling();
    BvhNode &getParent();
    BvhNode &getChild(const NodeType &type);
    void setType(NodeType &type);
    bool isRoot();
    BoundingBox &getBoundingBox();
    static BvhNode merge(BvhNode &left, BvhNode &right);
};
#endif