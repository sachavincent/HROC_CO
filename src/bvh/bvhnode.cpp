#include "bvh/bvhnode.hpp"
#include "bvh/boundingbox.hpp"
#include "bvh/boundingBoxObject.hpp"
#include "object.hpp"

BvhNode::BvhNode(std::shared_ptr<BoundingBox> _boundingBox, int _id)
    : boundingBox(_boundingBox), leftChild(nullptr), rightChild(nullptr), parent(nullptr), id(_id), tag(Visibility::null)
{
}

BvhNode::~BvhNode()
{
    boundingBox.reset();
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

std::vector<std::shared_ptr<const Object>> BvhNode::getObjectsInLeafs()
{
    std::vector<std::shared_ptr<const Object>> leafs;

    if (boundingBox->getObject())
        leafs.push_back(boundingBox->getObject());

    if (hasLeftChild())
    {
        std::vector<std::shared_ptr<const Object>> leftLeafs = leftChild->getObjectsInLeafs();
        leafs.insert(leafs.end(), leftLeafs.begin(), leftLeafs.end());
    }

    if (hasRightChild())
    {
        std::vector<std::shared_ptr<const Object>> rightLeafs = rightChild->getObjectsInLeafs();
        leafs.insert(leafs.end(), rightLeafs.begin(), rightLeafs.end());
    }

    return leafs;
}