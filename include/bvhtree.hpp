#ifndef BVHTREE_HPP
#define BVHTREE_HPP

#include <stdlib.h>

#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "bvhnode.hpp"
class BvhTree {
private:
    typedef std::pair<float, std::pair<BvhNode, BvhNode>> PairDistanceNode;
    typedef std::pair<BvhNode, BvhNode> PairNode;

    BvhNode *root;
    std::multimap<float, PairNode> *map;

public:
    BvhTree(std::vector<BoundingBox> &objs);

    std::vector<BvhNode> extractOccludees(std::vector<BvhNode> &allNodes);

    void createMap(std::vector<BvhNode> &nodes);

    void mergeAll(std::vector<BvhNode> &nodes);

    PairNode requestMap();

    void removeFromMap(BvhNode &node);

    void addToMap(BvhNode &node, std::vector<BvhNode> &nodesToCompare);

private:
    void eraseInVector(std::vector<BvhNode> &nodes, BvhNode node);
};

#endif