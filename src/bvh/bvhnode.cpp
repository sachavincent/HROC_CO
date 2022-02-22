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
    if(parent != nullptr)
    {
        if((BvhNode*)(parent->leftChild.get()) == this)
        {
            parent->leftChild = nullptr;
        }
        else
        {
            parent->rightChild = nullptr;
        }
    }
    
}

std::shared_ptr<BvhNode>BvhNode::getChild(const NodeType &_t) const
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

std::shared_ptr<BvhNode>BvhNode::sibling() const
{
    std::shared_ptr<BvhNode> _p(parent);
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

std::shared_ptr<BvhNode>BvhNode::merge(std::shared_ptr<BvhNode>_left, std::shared_ptr<BvhNode>_right, int _newId)
{
    auto ptrLeftBox = _left->getBoundingBox();
    auto ptrRightBox = _right->getBoundingBox();
    AxisBoundingBox *bbLeft = dynamic_cast<AxisBoundingBox *>(ptrLeftBox.get());
    AxisBoundingBox *bbRight = dynamic_cast<AxisBoundingBox *>(ptrRightBox.get());
    BoundingBox *bb = bbLeft->merge(bbRight);
    std::shared_ptr<BoundingBox> bbPtr(bb);
    std::shared_ptr<BvhNode> p(new BvhNode(bbPtr, _newId));
    p->leftChild = _left;
    p->rightChild = _right;

    _left->type = LEFT;
    _left->parent = p;

    _right->type = RIGHT;
    _right->parent = p;
    return p;
}

std::vector<std::shared_ptr<Object>> BvhNode::getObjectsInLeafs()
{
    std::vector<std::shared_ptr<Object>> leafs;

    if (boundingBox->getObject())
        leafs.push_back(boundingBox->getObject());

    if (hasLeftChild())
    {
        std::vector<std::shared_ptr<Object>> leftLeafs = leftChild->getObjectsInLeafs();
        leafs.insert(leafs.end(), leftLeafs.begin(), leftLeafs.end());
    }

    if (hasRightChild())
    {
        std::vector<std::shared_ptr<Object>> rightLeafs = rightChild->getObjectsInLeafs();
        leafs.insert(leafs.end(), rightLeafs.begin(), rightLeafs.end());
    }

    return leafs;
}