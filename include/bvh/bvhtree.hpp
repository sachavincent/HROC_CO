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
    typedef std::pair<float, std::pair<std::shared_ptr<BvhNode>, std::shared_ptr<BvhNode>>> PairDistanceNode;
    typedef std::pair<std::shared_ptr<BvhNode>, std::shared_ptr<BvhNode>> PairNode;

#ifdef HROC_TESTS
    BvhTree() : idGenerator(nullptr), map(nullptr), root(nullptr)
    {
    }

    std::multimap<float, PairNode> getMap() const { return *map; };
    std::shared_ptr<BvhNode> getRoot() { return root; }
#endif
private:
    std::shared_ptr<BvhNode> root;
    std::multimap<float, PairNode> *map;
    std::vector<std::shared_ptr<BvhNode>> nodes;
    IdGenerator *idGenerator;

public:
    BvhTree(std::vector<std::shared_ptr<BoundingBox>> &objs, IdGenerator *_idGen);
    BvhTree(std::vector<std::shared_ptr<BoundingBox>> &objs);

    ~BvhTree();

    std::vector<std::shared_ptr<BvhNode>> extractOccludees(const std::vector<std::shared_ptr<BvhNode>> &occluders);

    void createMap(std::vector<std::shared_ptr<BvhNode>> &nodes);

    void mergeAll(std::vector<std::shared_ptr<BvhNode>> &nodes);

    PairNode requestMap();

    void removeFromMap(BvhNode &node);

    void addToMap(std::shared_ptr<BvhNode>node, std::vector<std::shared_ptr<BvhNode>> &nodesToCompare);

    void printBT(const std::string &prefix, std::shared_ptr<BvhNode> node, bool isLeft)
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

    void printBT(std::shared_ptr<BvhNode> node)
    {
        printBT("", node, false);
    }

    void print()
    {
        printBT(root);
    }

    void nodeDepthExploration(std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> &nodeDepths, std::shared_ptr<BvhNode>node, int depth)
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
    static void eraseInVector(std::vector<std::shared_ptr<BvhNode>> &nodes, std::shared_ptr<BvhNode>node);

    void destroyRecursive(std::shared_ptr<BvhNode>node);
};

#endif