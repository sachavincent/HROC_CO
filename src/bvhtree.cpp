#include "bvhtree.hpp"
BvhTree::BvhTree(std::vector<BoundingBox> &objs)
{
    std::vector<BvhNode> nodes;
    nodes.reserve(objs.size());
    for (auto &bb : objs)
    {
        nodes.emplace_back(BvhNode(&bb));
    }
    mergeAll(nodes);
}
// vector<BvhNode> BvhTree::extractOccludees(vector<long int> &indices){}
void BvhTree::mergeAll(std::vector<BvhNode> &list)
{
    if (list.size() == 1)
    {
        root = &list[0];
        return;
    }
    std::pair<int, int> orderedNodes = orderNodes(list);

    BvhNode first = list[orderedNodes.first];
    BvhNode second = list[orderedNodes.second];
    BvhNode merged = BvhNode::merge(first, second);

    list.erase(list.begin() + orderedNodes.first);
    list.erase(list.begin() + orderedNodes.second - 1);
    list.push_back(merged);
    mergeAll(list);
}

std::pair<int, int> BvhTree::orderNodes(std::vector<BvhNode> &nodes)
{
    return std::make_pair(1, 2);
}