#include "bvhnode.hpp"
BvhNode::BvhNode(BoundingBox & bb){
    boundingBox = &bb;
    leftChild = nullptr;
    rightChild = nullptr;
    parent = nullptr;
}

BvhNode BvhNode::getLeftChild(){
    return *leftChild;
}

BvhNode BvhNode::getRightChild(){
    return *rightChild;
}

BvhNode BvhNode::getParent(){
    return *parent;
}
BvhNode BvhNode::getChild(const NodeType & t){
    if(t == LEFT){
        return getLeftChild();
    }
    else{
        return getRightChild();
    }
}
BvhNode BvhNode::sibling(){
    BvhNode * p = parent;
    if(type == LEFT){
        return p->getLeftChild();
    }
    else{
        return p->getRightChild();
    }
}
void BvhNode::setType(NodeType & type){
    this->type = type;
}

bool BvhNode::isRoot(){
    return &getParent() != nullptr;
}
BoundingBox BvhNode::getBoundingBox(){
    return *boundingBox;
}

BvhNode BvhNode::merge(BvhNode &left, BvhNode &right){
    BoundingBox bb = left.getBoundingBox().merge(right.getBoundingBox());
    BvhNode & parent = BvhNode(bb);
    parent.leftChild = &left;
    parent.rightChild = &right;

    left.type = LEFT;
    left.parent = &parent;

    right.type = RIGHT;
    right.parent = &parent;
}

