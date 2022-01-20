#include "bvhtree.hpp"
BvhTree::BvhTree(vector<BoundingBox> &objs)
{
    vector<BvhNode> nodes;
    nodes.reserve(objs.size());
    for (auto &bb : objs)
    {
        nodes.emplace_back(BvhNode(bb));
    }
    mergeAll(nodes);
}
// vector<BvhNode> BvhTree::extractOccludees(vector<long int> &indices){}
void BvhTree::mergeAll(vector<BvhNode> &list)
{
    if (list.size() == 1)
    {
        root = &list[0];
        return;
    }
    pair<int, int> orderedNodes = orderNodes(list);

    BvhNode first = list[orderedNodes.first];
    BvhNode second = list[orderedNodes.second];
    BvhNode merged = BvhNode::merge(first, second);

    list.erase(list.begin() + orderedNodes.first);
    list.erase(list.begin() + orderedNodes.second - 1);
    list.push_back(merged);
    mergeAll(list);
}
pair<int, int> BvhTree::orderNodes(vector<BvhNode> &nodes)
{
    return make_pair(1, 2);
}