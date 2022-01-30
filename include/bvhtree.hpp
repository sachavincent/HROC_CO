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

    void printBT(const std::string &prefix, const BvhNode &node, bool isLeft)
    {
        if (&node != nullptr)
        {
            std::cout << prefix;

            std::cout << (isLeft ? "├──" : "└──");

            // print the value of the node
            std::cout << node.getId() << std::endl;

            // enter the next tree level - left and right branch
            printBT(prefix + (isLeft ? "│   " : "    "), node.getLeftChild(), true);
            printBT(prefix + (isLeft ? "│   " : "    "), node.getRightChild(), false);
        }
    }

    void printBT(const BvhNode &node)
    {
        printBT("", node, false);
    }

    void print()
    {
        printBT(*root);
    }

private:
    static void eraseInVector(std::vector<BvhNode> &nodes, BvhNode node);
};

#endif