#include "bvhtree.hpp"
void eraseInVector(std::vector<BvhNode> & nodes, BvhNode node){
    for (auto it = nodes.begin();it!=nodes.end();it++){
        if(it->getId() == node.getId()){
            nodes.erase(it);
            return;
        }
    }
}
BvhTree::BvhTree(std::vector<BoundingBox> &objs)
{
    std::vector<BvhNode> nodes;
    nodes.reserve(objs.size());
    for (auto &bb : objs)
    {
        nodes.emplace_back(BvhNode(&bb));
    }
    createMap(nodes);
    mergeAll(nodes);
}
void BvhTree::createMap(std::vector<BvhNode> &nodes)
{   
    map = new std::multimap<float, PairNode>();
    
    map->insert(PairDistanceNode(BoundingBox::distance((nodes[0]).getBoundingBox(), (nodes[1]).getBoundingBox()),PairNode(nodes[0],nodes[1])));

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
        map->insert(PairDistanceNode(BoundingBox::distance((BoundingBox &)node.getBoundingBox(), (BoundingBox &)it->getBoundingBox()),PairNode(node, *it)));
    }
}
void BvhTree::removeFromMap(BvhNode &node)
{
    for (auto it = map->begin(); it != map->end(); it++)
    {
        if ((it->second.first.getId() == node.getId()) || (it->second.second.getId()) == node.getId())
        {
            map->erase(it);
            it--;
        }
    }
}
void BvhTree::mergeAll(std::vector<BvhNode> &nodes)
{
    PairNode pair = requestMap();
    BvhNode first = pair.first;
    BvhNode second = pair.second;
    BvhNode merged = BvhNode::merge(first, second);
    if (map->size() == 1)
    {
        root = &merged;
        return;
    }
    eraseInVector(nodes,first);
    eraseInVector(nodes,second);
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
