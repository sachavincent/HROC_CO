#include "gtest/gtest.h"

#include "bvh/boundingbox.hpp"
#include "bvh/bvhtree.hpp"
#include <stdexcept>
#include <memory>

class BvhTest : public ::testing::Test
{
protected:
    BvhTest()
    {
        glm::vec3 size = {1, 1, 1};
        std::vector<glm::vec3> centers1 = {
            {-6.5, 2.5, 0},
            {3.5, 2.5, 0},
            {3.5, 0.5, 0},
            {1.5, 1.5, 0},
            {-3.5, 2.5, 0},
            {4.5, 5.5, 0},
            {-1.5, 5.5, 0}};
        bbs1.reserve(centers1.size());
        for (auto const &value : centers1)
        {
            auto bb = std::make_shared<AxisBoundingBox>(value, size);
            bbs1.push_back(bb);
        }

        std::vector<glm::vec3> centers2 = {
            {1, 0, 0},
            {-1, 0, 0},
            {4, 0, 0},
            {10, 0, 0}};

        std::vector<glm::vec3> sizes2 = {
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1},
            {1, 1, 1}};

        simpleBBList.reserve(centers2.size());
        for (size_t i = 0; i < centers2.size(); i++)
        {
            auto simpleBB = std::make_shared<AxisBoundingBox>(centers2[i], sizes2[i]);
            simpleBBList.push_back(simpleBB);
        }
        // You can do set-up work for each test here.
    }
    virtual ~BvhTest()
    {
        // You can do clean-up work that doesn't throw exceptions here.
    }
    virtual void SetUp()
    {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    virtual void TearDown()
    {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    std::vector<std::shared_ptr<BoundingBox>> bbs1;
    std::vector<std::shared_ptr<BoundingBox>> simpleBBList;

    std::vector<std::shared_ptr<BoundingBox>> boundingBoxes;
};

void EXPECT_VEC3(glm::vec3 actual, glm::vec3 expected)
{
    EXPECT_EQ(actual[0], expected[0]);
    EXPECT_EQ(actual[1], expected[1]);
    EXPECT_EQ(actual[2], expected[2]);
}

/**
 * Tests BvhNode::merge with 2 BB of same size at different positions
 */
TEST_F(BvhTest, Merge_Case_1)
{
    IdGenerator idGenerator = IdGenerator();
    BvhNode *nodeLeft = new BvhNode(simpleBBList[0], idGenerator.GetUniqueId());
    BvhNode *nodeRight = new BvhNode(simpleBBList[1], idGenerator.GetUniqueId());

    ///////////////////////////////////////
    BvhNode *actual = BvhNode::merge(nodeLeft, nodeRight, idGenerator.GetUniqueId());
    ///////////////////////////////////////

    glm::vec3 expectedCenter = glm::vec3{0, 0, 0};
    glm::vec3 expectedSize = glm::vec3{3, 1, 1};

    EXPECT_VEC3(actual->getBoundingBox()->getCenter(), expectedCenter);
    EXPECT_VEC3(actual->getBoundingBox()->getSize(), expectedSize);
}

/**
 * Tests BvhNode::merge with 2 BB of different sizes at same position
 */
TEST_F(BvhTest, Merge_Case_2)
{
    IdGenerator idGenerator = IdGenerator();
    BvhNode *nodeLeft = new BvhNode(simpleBBList[2], idGenerator.GetUniqueId());
    auto bb = std::make_shared<AxisBoundingBox>(nodeLeft->getBoundingBox().get()->getCenter(), glm::vec3(2, 0.5, 4));
    BvhNode *nodeRight = new BvhNode(bb, idGenerator.GetUniqueId());

    ///////////////////////////////////////
    BvhNode *actual = BvhNode::merge(nodeLeft, nodeRight, idGenerator.GetUniqueId());
    ///////////////////////////////////////

    glm::vec3 expectedCenter = glm::vec3{4, 0, 0};
    glm::vec3 expectedSize = glm::vec3{2, 1, 4};

    EXPECT_VEC3(actual->getBoundingBox()->getCenter(), expectedCenter);
    EXPECT_VEC3(actual->getBoundingBox()->getSize(), expectedSize);
}

/**
 * Tests BvhNode::merge with 2 BB of different sizes and different positions
 */
TEST_F(BvhTest, Merge_Case_3)
{
    IdGenerator idGenerator = IdGenerator();
    auto bb1 = std::make_shared<AxisBoundingBox>(glm::vec3{-1, 3, -7}, glm::vec3(2, 3, 5));
    auto bb2 = std::make_shared<AxisBoundingBox>(glm::vec3(-4, -5, 2), glm::vec3(3, 2, 1));
    BvhNode *nodeLeft = new BvhNode(bb1, idGenerator.GetUniqueId());
    BvhNode *nodeRight = new BvhNode(bb2, idGenerator.GetUniqueId());

    ///////////////////////////////////////
    BvhNode *actual = BvhNode::merge(nodeLeft, nodeRight, idGenerator.GetUniqueId());
    ///////////////////////////////////////

    glm::vec3 expectedCenter = glm::vec3{-2.75, -0.75, -3.5};
    glm::vec3 expectedSize = glm::vec3{5.5, 10.5, 12};

    EXPECT_VEC3(actual->getBoundingBox()->getCenter(), expectedCenter);
    EXPECT_VEC3(actual->getBoundingBox()->getSize(), expectedSize);
}

/**
 * Tests BvhTree::CreateMap with global case
 */
TEST_F(BvhTest, CreateMap_Case_1)
{
    IdGenerator idGenerator = IdGenerator();
    std::vector<BvhNode *> nodes;
    nodes.reserve(simpleBBList.size());
    for (auto bb : simpleBBList)
    {
        nodes.push_back(new BvhNode(bb, idGenerator.GetUniqueId()));
    }

    BvhTree tree;

    ///////////////////////////////////////
    tree.createMap(nodes);
    ///////////////////////////////////////

    std::multimap<float, BvhTree::PairNode> actual = tree.getMap();
    std::vector<glm::vec3> centers2 = {
        {1, 0, 0},
        {-1, 0, 0},
        {4, 0, 0},
        {10, 0, 0}};
    std::vector<float> expected{2, 3, 5, 6, 9, 11};

    EXPECT_EQ(expected.size(), actual.size());

    int i = 0;
    for (auto it = actual.begin(); it != actual.end(); it++, i++)
    {
        EXPECT_EQ(expected[i], it->first);
    }
}

/**
 * Tests BvhTree::CreateMap with 1 element
 */
TEST_F(BvhTest, CreateMap_Case_2)
{
    IdGenerator idGenerator = IdGenerator();
    std::vector<BvhNode *> nodes = {new BvhNode(simpleBBList[0], idGenerator.GetUniqueId())};

    BvhTree tree;

    ///////////////////////////////////////
    tree.createMap(nodes);
    ///////////////////////////////////////

    std::multimap<float, BvhTree::PairNode> actual = tree.getMap();
    std::vector<float> expected{};

    EXPECT_EQ(expected.size(), actual.size());
    EXPECT_EQ(expected.size(), 0);
}

/**
 * Tests BvhTree::CreateMap with 0 element
 */
TEST_F(BvhTest, CreateMap_Case_3)
{
    std::vector<BvhNode *> nodes;

    BvhTree tree;

    ///////////////////////////////////////
    tree.createMap(nodes);
    ///////////////////////////////////////

    std::multimap<float, BvhTree::PairNode> actual = tree.getMap();
    std::vector<float> expected{};

    EXPECT_EQ(expected.size(), actual.size());
    EXPECT_EQ(expected.size(), 0);
}

/**
 * Tests BvhTree::CreateMap with 2 elements (bare minimum)
 */
TEST_F(BvhTest, CreateMap_Case_4)
{
    IdGenerator idGenerator = IdGenerator();
    std::vector<BvhNode *> nodes = {new BvhNode(simpleBBList[0], idGenerator.GetUniqueId()), new BvhNode(simpleBBList[1], idGenerator.GetUniqueId())};

    BvhTree tree;

    ///////////////////////////////////////
    tree.createMap(nodes);
    ///////////////////////////////////////

    std::multimap<float, BvhTree::PairNode> actual = tree.getMap();
    std::vector<glm::vec3> centers2 = {
        {1, 0, 0},
        {-1, 0, 0}};
    std::vector<float> expected{2};

    EXPECT_EQ(expected.size(), actual.size());

    int i = 0;
    for (auto it = actual.begin(); it != actual.end(); it++, i++)
    {
        EXPECT_EQ(expected[i], it->first);
    }
}

/**
 * @brief Parcours en profondeur
 */
void getTreeIds(BvhNode *root, std::vector<int> &idList)
{
    idList.push_back(root->getId());
    if (root->hasLeftChild())
        getTreeIds(root->getLeftChild(), idList);
    if (root->hasRightChild())
        getTreeIds(root->getRightChild(), idList);
}

/**
 * Tests BvhTree::BvhTree(std::shared_ptr<BoundingBox>) general case
 */
TEST_F(BvhTest, Constructor_Case_1)
{
    ///////////////////////////////////////
    BvhTree tree(bbs1);
    ///////////////////////////////////////

    BvhNode *root = tree.getRoot();

    std::vector<int> expectedIdList = {12, 11, 9, 4, 0, 6, 10, 8, 7, 2, 1, 3, 5};
    std::vector<int> actualList;

    getTreeIds(root, actualList);

    EXPECT_EQ(expectedIdList.size(), actualList.size());

    int i = 0;
    for (auto it = actualList.begin(); it != actualList.end(); it++, i++)
    {
        EXPECT_EQ(expectedIdList[i], *it);
    }
}

/**
 * Tests BvhTree::BvhTree(std::shared_ptr<BoundingBox>) for over overlapped BB but different sizes
 */
TEST_F(BvhTest, Constructor_Case_2)
{
    glm::vec3 center = {1, 1, 1};
    std::vector<glm::vec3> sizes = {
        {6.5, 2.5, 3},
        {3.5, 2.5, 4},
        {3.5, 0.5, 2},
        {1.5, 1.5, 1},
        {3.5, 2.5, 7},
        {4.5, 5.5, 2.5},
        {1.5, 5.5, 3}};
    boundingBoxes.reserve(sizes.size());
    for (auto const &value : sizes)
    {
        auto bb = std::make_shared<AxisBoundingBox>(center, value);
        boundingBoxes.push_back(bb);
    }

    ///////////////////////////////////////
    BvhTree tree(boundingBoxes);
    ///////////////////////////////////////

    BvhNode *root = tree.getRoot();

    std::vector<int> expectedIdList = {12, 11, 9, 5, 4, 8, 3, 2, 10, 7, 0, 1, 6};
    std::vector<int> actualList;

    getTreeIds(root, actualList);

    EXPECT_EQ(expectedIdList.size(), actualList.size());

    int i = 0;
    for (auto it = actualList.begin(); it != actualList.end(); it++, i++)
    {
        EXPECT_EQ(expectedIdList[i], *it);
    }
}

/**
 * Tests BvhTree::BvhTree(std::shared_ptr<BoundingBox>) for BB at equal distance
 */
TEST_F(BvhTest, Constructor_Case_3)
{
    glm::vec3 size = {1, 1, 1};
    std::vector<glm::vec3> centers;
    for (int i = 0; i < 15; i++)
    {
        centers.push_back(glm::vec3{i, i, i});
    }
    boundingBoxes.reserve(centers.size());
    for (auto const &value : centers)
    {
        auto bb = std::make_shared<AxisBoundingBox>(value, size);
        boundingBoxes.push_back(bb);
    }

    ///////////////////////////////////////
    BvhTree tree(boundingBoxes);
    ///////////////////////////////////////

    BvhNode *root = tree.getRoot();

    std::vector<int> expectedIdList = {28, 27, 24, 18, 7, 6, 17, 5, 4, 23, 16, 3, 2, 15, 0,
                                       1, 26, 25, 20, 11, 10, 19, 9, 8, 22, 21, 13, 12, 14};
    std::vector<int> actualList;

    getTreeIds(root, actualList);

    EXPECT_EQ(expectedIdList.size(), actualList.size());

    int i = 0;
    for (auto it = actualList.begin(); it != actualList.end(); it++, i++)
    {
        EXPECT_EQ(expectedIdList[i], *it);
    }
}

/**
 * Tests BvhTree::BvhTree(std::shared_ptr<BoundingBox>) with 1 boundingBox
 */
TEST_F(BvhTest, Constructor_Case_4)
{
    boundingBoxes.push_back(bbs1[1]);

    ///////////////////////////////////////
    BvhTree tree(boundingBoxes);
    ///////////////////////////////////////

    BvhNode *root = tree.getRoot();

    std::vector<int> actualList;

    getTreeIds(root, actualList);

    EXPECT_EQ(actualList.size(), 1);
    EXPECT_EQ(actualList[0], 0);
}