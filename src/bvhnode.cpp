#include "bvhnode.hpp"
#include "boundingbox.hpp"
#include "boundingBoxObject.hpp"

BvhNode::BvhNode(std::shared_ptr<BoundingBox> _boundingBox, int _id)
    : boundingBox(_boundingBox), leftChild(nullptr), rightChild(nullptr), parent(nullptr), id(_id), tag(Visibility::null)
{
}

BvhNode *BvhNode::getChild(const NodeType &_t) const
{
    if (_t == LEFT)
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
    BvhNode *_p = parent;
    if (type == LEFT)
    {
        return _p->getRightChild();
    }
    else
    {
        return _p->getLeftChild();
    }
}

bool BvhNode::isRoot()
{
    return getParent() != nullptr;
}

BvhNode *BvhNode::merge(BvhNode *_left, BvhNode *_right, int _newId)
{
    auto ptrLeftBox = _left->getBoundingBox();
    auto ptrRightBox = _right->getBoundingBox();
    AxisBoundingBox *bbLeft = dynamic_cast<AxisBoundingBox *>(ptrLeftBox.get());
    AxisBoundingBox *bbRight = dynamic_cast<AxisBoundingBox *>(ptrRightBox.get());
    BoundingBox *bb = bbLeft->merge(bbRight);
    bb->getWireframe()->load();
    std::shared_ptr<BoundingBox> bbPtr(bb);
    BvhNode *p = new BvhNode(bbPtr, _newId);
    p->leftChild = _left;
    p->rightChild = _right;

    _left->type = LEFT;
    _left->parent = p;

    _right->type = RIGHT;
    _right->parent = p;
    return p;
}
