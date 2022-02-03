#ifndef BVHTREE_HPP
#define BVHTREE_HPP

#include <stdlib.h>

#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "bvhnode.hpp"
#include "idgenerator.hpp"
class BvhTree
{

public: // TODO: For test purposes
    typedef std::pair<float, std::pair<BvhNode *, BvhNode *>> PairDistanceNode;
    typedef std::pair<BvhNode *, BvhNode *> PairNode;
    BvhTree() {}

    std::multimap<float, PairNode> *getMap() const { return map; };
    BvhNode *getRoot() { return root; }

private:
    // typedef std::pair<float, std::pair<BvhNode, BvhNode>> PairDistanceNode;
    // typedef std::pair<BvhNode, BvhNode> PairNode;

    BvhNode *root;
    std::multimap<float, PairNode> *map;
    std::vector<BvhNode *> nodes;
    IdGenerator *idGenerator = nullptr;

public:
    BvhTree(std::vector<BoundingBox *> &objs, IdGenerator *_idGen);
    BvhTree(std::vector<BoundingBox *> &objs);

    std::vector<BvhNode *> *extractOccludees(std::vector<BvhNode *> &allNodes);

    void createMap(std::vector<BvhNode *> &nodes);

    void mergeAll(std::vector<BvhNode *> &nodes);

    PairNode requestMap();

    void removeFromMap(BvhNode &node);

    void addToMap(BvhNode *node, std::vector<BvhNode *> &nodesToCompare);

    void printBT(const std::string &prefix, const BvhNode *node, bool isLeft)
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

    void printBT(const BvhNode *node)
    {
        printBT("", node, false);
    }

    void print()
    {
        printBT(root);
    }

    void nodeDepthExploration(std::map<int, std::vector<BoundingBoxObject *>> &nodeDepths, const BvhNode *node, int depth)
    {
        if (nodeDepths.find(depth) == nodeDepths.end())
            nodeDepths.insert(std::make_pair(depth, std::vector<BoundingBoxObject *>()));

        nodeDepths[depth].push_back(node->getBoundingBox().getWireframe());

        if (node->hasLeftChild())
            nodeDepthExploration(nodeDepths, node->getLeftChild(), depth + 1);
        if (node->hasRightChild())
            nodeDepthExploration(nodeDepths, node->getRightChild(), depth + 1);
    }

    std::map<int, std::vector<BoundingBoxObject *>> getDebugData()
    {
        std::map<int, std::vector<BoundingBoxObject *>> nodeDepths;
        nodeDepthExploration(nodeDepths, root, 0);

        return nodeDepths;
    }

private:
    static void eraseInVector(std::vector<BvhNode *> &nodes, BvhNode *node);
};

#endif