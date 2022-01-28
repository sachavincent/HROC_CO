#ifndef BVHTREE_HPP
#define BVHTREE_HPP

#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>
#include "bvhnode.hpp"
#include <utility>
class BvhTree
{


private:

    typedef std::pair<float,std::pair<BvhNode,BvhNode>> PairDistanceNode;
    typedef std::pair<BvhNode,BvhNode> PairNode;
    BvhNode *root;
    std::multimap<float,PairNode> * map;
public:
    BvhTree(std::vector<BoundingBox> &objs);

    std::vector<BvhNode> BvhTree::extractOccludees(std::vector<BvhNode> &allNodes);
    void createMap(std::vector<BvhNode> &nodes);
    void mergeAll(std::vector<BvhNode> &nodes);
    
    PairNode requestMap();
    void removeFromMap(BvhNode &node);
    void addToMap(BvhNode & node, std::vector<BvhNode> &nodesToCompare);

};

#endif