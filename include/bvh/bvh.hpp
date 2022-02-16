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

#include "boundingBoxObject.hpp"
#include "bounding_box.hpp"
#include "utilities.hpp"

namespace bvh
{

    enum Visibility
    {
        null,
        VISIBLE,
        POTENTIAL_OCCLUDERS,
        UNKNOWN
    };
    /// This structure represents a BVH with a list of nodes and primitives indices.
    /// The memory layout is such that the children of a node are always grouped together.
    /// This means that each node only needs one index to point to its children, as the other
    /// child can be obtained by adding one to the index of the first child. The root of the
    /// hierarchy is located at index 0 in the array of nodes.
    struct Bvh
    {
        using IndexType = typename SizedIntegerType<sizeof(float) * CHAR_BIT>::Unsigned;

        // The size of this structure should be 32 bytes in
        // single precision and 64 bytes in double precision.
        struct Node
        {
            Visibility tag = Visibility::null;
            std::shared_ptr<BoundingBox> boundingBox;
            float bounds[6];
            IndexType primitive_count;
            IndexType first_child_or_primitive;
            IndexType parent;

            bool is_leaf() const { return primitive_count != 0; }

            inline const Visibility getVisibility() const { return tag; }
            inline void setVisibility(const Visibility &_tag)
            {
                tag = _tag;
            }
            /// Accessor to simplify the manipulation of the bounding box of a node.
            /// This type is convertible to a `BoundingBox`.
            struct BoundingBoxProxy
            {
                Node &node;

                BoundingBoxProxy(Node &node)
                    : node(node)
                {
                }

                BoundingBoxProxy &operator=(const BoundingBox &bbox)
                {
                    node.bounds[0] = bbox.minV[0];
                    node.bounds[1] = bbox.maxV[0];
                    node.bounds[2] = bbox.minV[1];
                    node.bounds[3] = bbox.maxV[1];
                    node.bounds[4] = bbox.minV[2];
                    node.bounds[5] = bbox.maxV[2];
                    return *this;
                }

                operator AxisBoundingBox() const
                {
                    return AxisBoundingBox(
                        glm::vec3(node.bounds[0], node.bounds[2], node.bounds[4]),
                        glm::vec3(node.bounds[1], node.bounds[3], node.bounds[5]));
                }

                AxisBoundingBox to_bounding_box() const
                {
                    return static_cast<AxisBoundingBox>(*this);
                }

                float half_area() const { return to_bounding_box().half_area(); }

                BoundingBoxProxy &extend(const BoundingBox &bbox)
                {
                    return *this = to_bounding_box().extend(bbox);
                }
            };

            BoundingBoxProxy bounding_box_proxy()
            {
                return BoundingBoxProxy(*this);
            }

            const BoundingBoxProxy bounding_box_proxy() const
            {
                return BoundingBoxProxy(*const_cast<Node *>(this));
            }
        };

        std::unique_ptr<Node[]> nodes;
        std::unique_ptr<size_t[]> primitive_indices;

        size_t node_count = 0;

        ~Bvh()
        {
            
        }
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

        void nodeDepthExploration(std::map<int, std::vector<std::shared_ptr<BoundingBoxObject>>, std::greater<int>> &nodeDepths, Node *node, int depth)
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

        std::vector<unsigned int> extractOccludees(const std::vector<unsigned int> &occluders)
        {
            std::vector<unsigned int> occludeeGroups;
            occludeeGroups.reserve(occluders.size());
            if (occluders.empty())
            {
                occludeeGroups.push_back(0);
                return occludeeGroups;
            }
            for (auto it = occluders.begin(); it != occluders.end(); it++)
            {
                nodes[*it].setVisibility(Visibility::null);
            }

            for (auto it = occluders.begin(); it != occluders.end(); it++)
            {
                Node &n = nodes[*it];
                while (n.getVisibility() != Visibility::VISIBLE)
                {
                    n.setVisibility(Visibility::VISIBLE);
                    Node &n2 = nodes[sibling(*it)];
                    n2.setVisibility(Visibility::UNKNOWN);
                    n = nodes[n.parent];
                    if (n.parent == 0) // n = root
                        break;
                }
            }
            for (auto it = occluders.begin(); it != occluders.end(); it++)
            {
                Node &n = nodes[*it];
                if (n.getVisibility() == Visibility::UNKNOWN)
                {
                    occludeeGroups.push_back(*it);
                }
            }

            return occludeeGroups;
        }
    };

} // namespace bvh

#endif
