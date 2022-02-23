#ifndef BVH_BVH_HPP
#define BVH_BVH_HPP

#include <climits>
#include <memory>
#include <vector>
#include <map>
#include <cassert>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "bvhnode.hpp"
#include "boundingBoxObject.hpp"
#include "bounding_box.hpp"
#include "utilities.hpp"

/// This structure represents a BVH with a list of nodes and primitives indices.
/// The memory layout is such that the children of a node are always grouped together.
/// This means that each node only needs one index to point to its children, as the other
/// child can be obtained by adding one to the index of the first child. The root of the
/// hierarchy is located at index 0 in the array of nodes.

struct Bvh
{
    std::unique_ptr<BvhNode[]> nodes;
    std::unique_ptr<size_t[]> primitive_indices;

    size_t node_count = 0;

    ~Bvh()
    {
    }

    inline const std::unique_ptr<BvhNode[]> &getNodes() { return nodes; }
    /// Given a node index, returns the index of its sibling.
    static size_t sibling(size_t index)
    {
        assert(index != 0);
        return index % 2 == 1 ? index + 1 : index - 1;
    }

    /// Returns true if the given node is the left sibling of another.
    static bool is_left_sibling(size_t index)
    {
        assert(index != 0);
        return index % 2 == 1;
    }

    void printBT(BvhNode node)
    {
        printBT("", node, false);
    }

    void print()
    {
        printBT(nodes[0]);
    }
    void printBT(const std::string &prefix, BvhNode node, bool isLeft)
    {
        std::cout << prefix;

        std::cout << (isLeft ? "|--" : "'--");

        // print the value of the node
        std::cout << node.getId() << std::endl;

        // enter the next tree level - left and right branch

        if (node.hasLeftChild(nodes))
            printBT(prefix + (isLeft ? "|   " : "    "), node.getLeftChild(nodes), true);
        if (node.hasRightChild(nodes))
            printBT(prefix + (isLeft ? "|   " : "    "), node.getRightChild(nodes), false);
    }
    void nodeDepthExploration(std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> &nodeDepths, BvhNode *node, int depth)
    {
        if (nodeDepths.find(depth) == nodeDepths.end())
            nodeDepths.insert(std::make_pair(depth, std::vector<std::shared_ptr<BoundingBoxObject>>()));

        AxisBoundingBox abb = node->bounding_box_proxy().to_bounding_box();
        nodeDepths[depth].push_back(abb.getWireframe());
        if (node->is_leaf())
            return;
        auto *leftChild = &nodes[node->first_child_or_primitive];
        auto *rightChild = leftChild + 1;
        if (leftChild)
            nodeDepthExploration(nodeDepths, leftChild, depth + 1);
        if (rightChild)
            nodeDepthExploration(nodeDepths, rightChild, depth + 1);
    }

    std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> getDebugData()
    {
        std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> nodeDepths;
        nodeDepthExploration(nodeDepths, &nodes[0], 0);

        return nodeDepths;
    }

    std::vector<std::shared_ptr<BvhNode>> extractOccludees(const std::vector<std::shared_ptr<BvhNode>> &occluders)
    {
        std::vector<std::shared_ptr<BvhNode>> occludeeGroups;

        occludeeGroups.reserve(occluders.size());
        if (occluders.empty())
        {
            nodes[0].setVisibility(Visibility::VISIBLE);
            occludeeGroups.push_back(std::shared_ptr<BvhNode>(&nodes[0]));
            return occludeeGroups;
        }
        nodes[0].setVisibilityRecursive(Visibility::null, nodes);

        for (auto it = occluders.begin(); it != occluders.end(); it++)
        {
            BvhNode &n = *((*it).get());
            while (n.getVisibility() != Visibility::VISIBLE)
            {
                n.setVisibility(Visibility::VISIBLE);
                BvhNode siblingNode = nodes[nodes[n.parent].first_child_or_primitive];
                if (n.getId() == siblingNode.getId())
                    siblingNode = nodes[sibling(nodes[n.parent].first_child_or_primitive)];
                BvhNode &n2 = siblingNode;
                n2.setVisibility(Visibility::UNKNOWN);
                n = nodes[n.parent];
                if (n.parent == 0) // n = root
                    break;
            }
        }

        nodes[0].setVisibility(Visibility::UNKNOWN);
        nodes[0].RecursiveAdd(occludeeGroups, nodes);
        return occludeeGroups;
    }
};

#endif
