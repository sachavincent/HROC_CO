#include "bvh/bvhtree.hpp"

BvhTree::BvhTree(std::vector<std::shared_ptr<BoundingBox>> &objs) : BvhTree(objs, new IdGenerator())
{
}

BvhTree::BvhTree(std::vector<std::shared_ptr<BoundingBox>> &objs, IdGenerator *_idGenerator) : root(nullptr), map(nullptr), idGenerator(_idGenerator)
{
    nodes.clear();
    nodes.reserve(objs.size());
    for (auto &bb : objs)
    {
        nodes.emplace_back(new BvhNode(bb, idGenerator->GetUniqueId()));
    }

    if (nodes.size() == 1) // Only one node (=root)
    {
        root = nodes[0];
    }
    else
    {
        createMap(nodes);
        mergeAll(nodes);
    }
}

BvhTree::~BvhTree()
{
    if (map)
    {
        map->clear();
        delete map;
    }

    if (idGenerator)
        delete idGenerator;

    destroyRecursive(root);

    nodes.clear();
}

void BvhTree::destroyRecursive(BvhNode *node)
{
    if (node)
    {
        destroyRecursive(node->getLeftChild());
        destroyRecursive(node->getRightChild());
        delete node;
    }
}

void BvhTree::createMap(std::vector<BvhNode *> &_nodes)
{
    map = new std::multimap<float, PairNode>();

    if (_nodes.size() < 2)
        return;

    map->insert(PairDistanceNode(BoundingBox::distance((_nodes[0])->getBoundingBox(), (_nodes[1])->getBoundingBox()), PairNode(_nodes[0], _nodes[1])));

    std::vector<BvhNode *> nodeInMap;
    nodeInMap.reserve(_nodes.size());
    nodeInMap.push_back(_nodes[0]);
    nodeInMap.push_back(_nodes[1]);
    for (auto it = _nodes.begin() + 2; it != _nodes.end(); ++it)
    {
        addToMap(*it, nodeInMap);
        nodeInMap.emplace_back(*it);
    }
}

void BvhTree::addToMap(BvhNode *node, std::vector<BvhNode *> &nodesToCompare)
{
    for (auto it = nodesToCompare.begin(); it != nodesToCompare.end(); ++it)
    {
        map->insert(PairDistanceNode(BoundingBox::distance(node->getBoundingBox(), (*it)->getBoundingBox()), PairNode(node, *it)));
    }
}

void BvhTree::removeFromMap(BvhNode &node)
{
    // std::cout << "B4 remove " << map->size() << std::endl;
    for (auto it = map->begin(); it != map->end();)
    {
        if ((it->second.first->getId() == node.getId()) || (it->second.second->getId()) == node.getId())
        {
            // std::cout << "before -- " << std::endl;
            map->erase(it++);
            // std::cout << "in --" << std::endl;

            // std::cout << "after --" << std::endl;
        }
        else
        {
            ++it;
        }
    }

    // std::cout << "After remove : " << map->size() << std::endl;
}

void BvhTree::mergeAll(std::vector<BvhNode *> &_nodes)
{
    if (map->empty())
        return;

    PairNode pair = requestMap();
    BvhNode *first = pair.first;
    BvhNode *second = pair.second;
    BvhNode *merged = BvhNode::merge(first, second, idGenerator->GetUniqueId());
    if (map->size() == 1)
    {
        root = merged;
        return;
    }
    eraseInVector(_nodes, first);
    eraseInVector(_nodes, second);
    removeFromMap(*first);
    removeFromMap(*second);
    addToMap(merged, _nodes);
    _nodes.push_back(merged);
    mergeAll(_nodes);
}

BvhTree::PairNode BvhTree::requestMap()
{
    auto it = map->begin();
    return it->second;
}

std::vector<BvhNode *> BvhTree::extractOccludees(const std::vector<BvhNode *> &occluders)
{
    std::vector<BvhNode *> occludeeGroups;
    occludeeGroups.reserve(occluders.size());
    if (occluders.empty())
    {
        occludeeGroups.push_back(root);
        return occludeeGroups;
    }
    for (auto it = occluders.begin(); it != occluders.end(); it++)
    {
        (*it)->setVisibility(Visibility::null);
    }

    for (auto it = occluders.begin(); it != occluders.end(); it++)
    {
        BvhNode *n = (*it);
        while (n->getVisibility() != Visibility::VISIBLE && n->getId() != root->getId())
        {
            n->setVisibility(Visibility::VISIBLE);
            BvhNode *n2 = n->sibling();
            n2->setVisibility(Visibility::UNKNOWN);
            n = n->getParent();
        }
    }
    for (auto it = occluders.begin(); it != occluders.end(); it++)
    {
        if ((*it)->getVisibility() == Visibility::UNKNOWN)
        {
            occludeeGroups.push_back(*it);
        }
    }

    return occludeeGroups;
}

void BvhTree::eraseInVector(std::vector<BvhNode *> &nodes, BvhNode *node)
{
    for (auto it = nodes.begin(); it != nodes.end(); it++)
    {
        if ((*it)->getId() == node->getId())
        {
            nodes.erase(it);
            return;
        }
    }
}