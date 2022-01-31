#include "bvhnode.hpp"
BvhNode::BvhNode(const BoundingBox *boundingBox,int _id) : _boundingBox(boundingBox), _leftChild(nullptr), _rightChild(nullptr), _parent(nullptr)
{
    id = _id;
}

BvhNode *BvhNode::getChild(const NodeType &t) const
{
    if (t == LEFT)
    {
        return getLeftChild();
    }
    else
    {
        return getRightChild();
    }
}

 BvhNode *BvhNode::sibling() const
{
    BvhNode *p = _parent;
    if (_type == LEFT)
    {
        return p->getRightChild();
    }
    else
    {
        return p->getLeftChild();
    }
}

bool BvhNode::isRoot()
{
    return getParent() != nullptr;
}

BvhNode* BvhNode::merge(BvhNode *left, BvhNode *right,int newid)
{
    const BoundingBox &bbLeft = left->getBoundingBox();
    const BoundingBox &bbRight = right->getBoundingBox();
    const BoundingBox *bb = bbLeft.merge(bbRight);
    BvhNode* p = new BvhNode(bb,newid);
    p->_leftChild = left;
    p->_rightChild = right;

    left->_type = LEFT;
    left->_parent = p;

    right->_type = RIGHT;
    right->_parent = p;
    return p;
}
