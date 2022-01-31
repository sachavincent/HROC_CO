#include "bvhtree.hpp"

BvhTree::BvhTree(std::vector<BoundingBox *> &objs)
{
    idGenerator = new IdGenerator();
    nodes.clear();
    nodes.reserve(objs.size());
    for (auto &bb : objs)
    {
        nodes.emplace_back(BvhNode(bb,idGenerator->GetUniqueId()));
    }
    createMap(nodes);
    mergeAll(nodes);
}


BvhTree::BvhTree(std::vector<BoundingBox *> &objs,IdGenerator* _idGenerator)
{
    idGenerator = _idGenerator;
    nodes.clear();
    nodes.reserve(objs.size());
    for (auto &bb : objs)
    {
        nodes.emplace_back(BvhNode(bb,idGenerator->GetUniqueId()));
    }
    createMap(nodes);
    mergeAll(nodes);
}

void BvhTree::createMap(std::vector<BvhNode> &nodes)
{
    map = new std::multimap<float, PairNode>();

    if (nodes.size() < 2)
        return;

    map->insert(PairDistanceNode(BoundingBox::distance((nodes[0]).getBoundingBox(), (nodes[1]).getBoundingBox()), PairNode(nodes[0], nodes[1])));

    std::vector<BvhNode> nodeInMap;
    nodeInMap.reserve(nodes.size());
    nodeInMap.push_back(nodes[0]);
    nodeInMap.push_back(nodes[1]);
    for (auto it = nodes.begin() + 2; it != nodes.end(); ++it)
    {
        addToMap(*it, nodeInMap);
        nodeInMap.push_back(*it);
    }
}

void BvhTree::addToMap(BvhNode &node, std::vector<BvhNode> &nodesToCompare)
{
    for (auto it = nodesToCompare.begin(); it != nodesToCompare.end(); ++it)
    {
        map->insert(PairDistanceNode(BoundingBox::distance((BoundingBox &)node.getBoundingBox(), (BoundingBox &)it->getBoundingBox()), PairNode(node, *it)));
    }
}

void BvhTree::removeFromMap(BvhNode &node)
{
    // std::cout << "B4 remove " << map->size() << std::endl;
    for (auto it = map->begin(); it != map->end(); it)
    {
        if ((it->second.first.getId() == node.getId()) || (it->second.second.getId()) == node.getId())
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

void BvhTree::mergeAll(std::vector<BvhNode> &nodes)
{
    PairNode pair = requestMap();
    BvhNode first = pair.first;
    BvhNode second = pair.second;
    BvhNode merged = BvhNode::merge(&first, &second,idGenerator->GetUniqueId());
    if (map->size() == 1)
    {
        root = &merged;
        return;
    }
    eraseInVector(nodes, first);
    eraseInVector(nodes, second);
    removeFromMap(first);
    removeFromMap(second);
    addToMap(merged, nodes);
    nodes.push_back(merged);
    mergeAll(nodes);
}

BvhTree::PairNode BvhTree::requestMap()
{
    auto it = map->begin();
    return it->second;
}

std::vector<BvhNode> BvhTree::extractOccludees(std::vector<BvhNode> &allNodes)
{

    std::vector<BvhNode> occludeeGroups;
    occludeeGroups.reserve(allNodes.size());
    if (allNodes.empty())
    {
        occludeeGroups.push_back(*root);
        return occludeeGroups;
    }
    for (auto it = allNodes.begin(); it != allNodes.end(); it++)
    {
        it->setVisibility(Visibility::null);
    }

    for (auto it = allNodes.begin(); it != allNodes.end(); it++)
    {
        BvhNode *n = &(*it);
        while (n->getVisibility() != Visibility::VISIBLE && n->getId() != root->getId())
        {
            n->setVisibility(Visibility::VISIBLE);
            BvhNode *n2 = n->sibling();
            n2->setVisibility(Visibility::UNKNOWN);
            n = n->getParent();
        }
    }
    for (auto it = allNodes.begin(); it != allNodes.end(); it++)
    {
        if (it->getVisibility() == Visibility::UNKNOWN)
        {
            occludeeGroups.push_back(*it);
        }
    }

    return occludeeGroups;
}

void BvhTree::eraseInVector(std::vector<BvhNode> &nodes, BvhNode node)
{
    for (auto it = nodes.begin(); it != nodes.end(); it++)
    {
        if (it->getId() == node.getId())
        {
            nodes.erase(it);
            return;
        }
    }
}