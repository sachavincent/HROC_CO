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
    BvhNode *root;

public:
    BvhTree(std::vector<BoundingBox> &objs);

    std::vector<BvhNode> extractOccludees(std::vector<long int> &indices);

    void mergeAll(std::vector<BvhNode> &list);
    
    std::pair<int, int> orderNodes(std::vector<BvhNode> &nodes);
};

#endif