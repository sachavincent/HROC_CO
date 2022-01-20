#ifndef BVHTREE_HPP
#define BVHTREE_HPP
#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>
#include "bvhnode.hpp"
#include <utility>
using namespace std;
class BvhTree
{
private:
    BvhNode *root;

public:
    BvhTree(vector<BoundingBox> &objs);
    vector<BvhNode> extractOccludees(vector<long int> &indices);
    void mergeAll(vector<BvhNode> &list);
    pair<int, int> orderNodes(vector<BvhNode> &nodes);
};
#endif