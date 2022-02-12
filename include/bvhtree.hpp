#ifndef BVHTREE_HPP
#define BVHTREE_HPP

#include <stdlib.h>

#include <iostream>
#include <map>
#include <utility>
#include <memory>
#include <vector>

#include "bvhnode.hpp"
#include "utils/idgenerator.hpp"
#include "boundingbox.hpp"

class BvhTree
{

#ifdef HROC_TESTS
public:
#else
private:
#endif
    typedef std::pair<float, std::pair<BvhNode *, BvhNode *>> PairDistanceNode;
    typedef std::pair<BvhNode *, BvhNode *> PairNode;

#ifdef HROC_TESTS
    BvhTree(): idGenerator(nullptr), map(nullptr), root(nullptr)
    {
    }

    std::multimap<float, PairNode> getMap() const { return *map; };
    BvhNode *getRoot() { return root; }
#endif
private:
    BvhNode *root;
    std::multimap<float, PairNode> *map;
    std::vector<BvhNode *> nodes;
    IdGenerator *idGenerator;

public:
    BvhTree(std::vector<std::shared_ptr<BoundingBox>> &objs, IdGenerator *_idGen);
    BvhTree(std::vector<std::shared_ptr<BoundingBox>> &objs);

    ~BvhTree();

    void destroyRecursive(BvhNode* node);

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

    void nodeDepthExploration(std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> &nodeDepths, BvhNode *node, int depth)
    {
        if (nodeDepths.find(depth) == nodeDepths.end())
            nodeDepths.insert(std::make_pair(depth, std::vector<std::shared_ptr<BoundingBoxObject>>()));
        nodeDepths[depth].push_back(std::shared_ptr<BoundingBoxObject>(node->getBoundingBox()->getWireframe()));

        if (node->hasLeftChild())
            nodeDepthExploration(nodeDepths, node->getLeftChild(), depth + 1);
        if (node->hasRightChild())
            nodeDepthExploration(nodeDepths, node->getRightChild(), depth + 1);
    }

    std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> getDebugData()
    {
        std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> nodeDepths;
        if (root)
            nodeDepthExploration(nodeDepths, root, 0);

        return nodeDepths;
    }

private:
    static void eraseInVector(std::vector<BvhNode *> &nodes, BvhNode *node);
};

#endif