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
        BvhNode *node = new BvhNode(bb, idGenerator->GetUniqueId());
        nodes.emplace_back(node);
#ifndef HROC_TESTS
        std::shared_ptr<BvhNode> ptrNode(node);
        bb->setNode(ptrNode);
#endif
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
    root = std::shared_ptr<BvhNode>(nullptr);
    nodes.clear();
}

void BvhTree::destroyRecursive(std::shared_ptr<BvhNode> node)
{
    if (node != nullptr)
    {
        std::string str;
        // printBT(str,root, true);
        destroyRecursive(node->getLeftChild());
        // node->
        // str.clear();
        // printBT(str, root, true);
        destroyRecursive(node->getRightChild());
        // str.clear();
        // printBT(str, root, true);
        node.~shared_ptr();
    }
}

void BvhTree::createMap(std::vector<std::shared_ptr<BvhNode>> &_nodes)
{
    map = new std::multimap<float, PairNode>();
    if (_nodes.size() < 2)
        return;

    map->insert(PairDistanceNode(BoundingBox::distance((_nodes[0])->getBoundingBox(), (_nodes[1])->getBoundingBox()), PairNode(_nodes[0], _nodes[1])));

    std::vector<std::shared_ptr<BvhNode>> nodeInMap;
    nodeInMap.reserve(_nodes.size());
    nodeInMap.push_back(_nodes[0]);
    nodeInMap.push_back(_nodes[1]);
    #pragma omp for
    for (auto it = _nodes.begin() + 2; it != _nodes.end(); ++it)
    {
        addToMap(*it, nodeInMap);
        nodeInMap.emplace_back(*it);
    }
    #pragma omp barrier
}

void BvhTree::addToMap(std::shared_ptr<BvhNode> node, std::vector<std::shared_ptr<BvhNode>> &nodesToCompare)
{
    #pragma omp for
    for (auto it = nodesToCompare.begin(); it != nodesToCompare.end(); ++it)
    {
        map->insert(PairDistanceNode(BoundingBox::distance(node->getBoundingBox(), (*it)->getBoundingBox()), PairNode(node, *it)));
    }
    
    #pragma omp barrier
}

void BvhTree::removeFromMap(BvhNode &node)
{
    // std::cout << "B4 remove " << map->size() << std::endl;
    #pragma omp for
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
    #pragma omp barrier

    // std::cout << "After remove : " << map->size() << std::endl;
}

void BvhTree::mergeAll(std::vector<std::shared_ptr<BvhNode>> &_nodes)
{
    if (map->empty())
        return;

    PairNode pair = requestMap();
    std::shared_ptr<BvhNode> first = pair.first;
    std::shared_ptr<BvhNode> second = pair.second;
    std::shared_ptr<BvhNode> merged = BvhNode::merge(first, second, idGenerator->GetUniqueId());
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

std::vector<std::shared_ptr<BvhNode>> BvhTree::extractOccludees(const std::vector<std::shared_ptr<BvhNode>> &occluders)
{
    std::vector<std::shared_ptr<BvhNode>> occludeeGroups;

    occludeeGroups.reserve(occluders.size());
    if (occluders.empty())
    {
        root->setVisibility(Visibility::VISIBLE);
        occludeeGroups.push_back(std::shared_ptr<BvhNode>(root));
        return occludeeGroups;
    }
    root->setVisibilityRecursive(Visibility::null);
    for (auto it = occluders.begin(); it != occluders.end(); it++)
    {
        std::shared_ptr<BvhNode> n = (*it);
        while (n->getVisibility() != Visibility::VISIBLE && n->getId() != root->getId())
        {
            n->setVisibility(Visibility::VISIBLE);
            std::shared_ptr<BvhNode> n2 = n->sibling();
            n2->setVisibility(Visibility::UNKNOWN);
            n = n->getParent();
        }       
    }
    root->setVisibility(Visibility::UNKNOWN);
    root->RecursiveAdd(occludeeGroups);
    return occludeeGroups;
}

void BvhTree::eraseInVector(std::vector<std::shared_ptr<BvhNode>> &nodes, std::shared_ptr<BvhNode> node)
{
    #pragma omp for
    for (auto it = nodes.begin(); it != nodes.end(); it++)
    {
        if ((*it)->getId() == node->getId())
        {
            nodes.erase(it);
            return;
        }
    }
    #pragma omp barrier
}