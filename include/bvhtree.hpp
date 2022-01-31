#ifndef BVHTREE_HPP
#define BVHTREE_HPP

#include <stdlib.h>

#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "bvhnode.hpp"
class BvhTree
{

public: // TODO: For test purposes
    typedef std::pair<float, std::pair<BvhNode, BvhNode>> PairDistanceNode;
    typedef std::pair<BvhNode, BvhNode> PairNode;
    BvhTree() {}

    std::multimap<float, PairNode> *getMap() const { return map; };
private:
    //typedef std::pair<float, std::pair<BvhNode, BvhNode>> PairDistanceNode;
    //typedef std::pair<BvhNode, BvhNode> PairNode;

    BvhNode *root;
    std::multimap<float, PairNode> *map;
    std::vector<BvhNode> nodes;

public:
    BvhTree(std::vector<BoundingBox *> &objs);

    std::vector<BvhNode> extractOccludees(std::vector<BvhNode> &allNodes);

    void createMap(std::vector<BvhNode> &nodes);

    void mergeAll(std::vector<BvhNode> &nodes);

    PairNode requestMap();

    void removeFromMap(BvhNode &node);

    void addToMap(BvhNode &node, std::vector<BvhNode> &nodesToCompare);

    void printBT(const std::string &prefix, const BvhNode  * node, bool isLeft)
    {
        if (node == nullptr)
            return;
        std::cout << prefix;

        std::cout << (isLeft ? "|--" : "'--");

        // print the value of the node
        std::cout << node->getId() << std::endl;

        // enter the next tree level - left and right branch

        if (node->hasLeftChild())
            printBT(prefix + (isLeft ? "|   " : "    "), node->getLeftChild(), true);
        if (node->hasRightChild())
            printBT(prefix + (isLeft ? "|   " : "    "), node->getRightChild(), false);
    }

    void printBT(const BvhNode * node)
    {
        printBT("", node, false);
    }

    void print()
    {
        printBT(root);
    }

private:
    static void eraseInVector(std::vector<BvhNode> &nodes, BvhNode node);
};

#endif