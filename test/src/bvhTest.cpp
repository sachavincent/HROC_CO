#include "gtest/gtest.h"

#include "boundingbox.hpp"
#include "bvhtree.hpp"
#include <stdexcept>

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
            bbs1.push_back(new AxisBoundingBox(value, size));
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
            simpleBBList.push_back(new AxisBoundingBox(centers2[i], sizes2[i]));
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

    std::vector<BoundingBox *> bbs1;
    std::vector<BoundingBox *> simpleBBList;
};

void EXPECT_VEC3(glm::vec3 actual, glm::vec3 expected)
{
    EXPECT_EQ(actual[0], expected[0]);
    EXPECT_EQ(actual[1], expected[1]);
    EXPECT_EQ(actual[2], expected[2]);
}
/*

BvhNode BvhNode::merge(BvhNode *left, BvhNode *right)
{
    const BoundingBox &bbLeft = left->getBoundingBox();
    const BoundingBox &bbRight = right->getBoundingBox();
    const BoundingBox *bb = bbLeft.merge(bbRight);
    BvhNode p = BvhNode(bb);
    p._leftChild = left;
    p._rightChild = right;

    left->_type = LEFT;
    left->_parent = &p;

    right->_type = RIGHT;
    right->_parent = &p;
    return p;
}
*/

/**
 * Tests BvhNode::merge with 2 BB of same size at different positions
 */
TEST_F(BvhTest, Merge_Case_1)
{
    IdGenerator idGenerator = IdGenerator();
    BvhNode *nodeLeft = new BvhNode(simpleBBList[0],idGenerator.GetUniqueId());
    BvhNode *nodeRight = new BvhNode(simpleBBList[1],idGenerator.GetUniqueId());

    ///////////////////////////////////////
    BvhNode* actual = BvhNode::merge(nodeLeft, nodeRight,idGenerator.GetUniqueId());
    ///////////////////////////////////////

    glm::vec3 expectedCenter = glm::vec3{0, 0, 0};
    glm::vec3 expectedSize = glm::vec3{3, 1, 1};

    EXPECT_VEC3(actual->getBoundingBox().getCenter(), expectedCenter);
    EXPECT_VEC3(actual->getBoundingBox().getSize(), expectedSize);
}

/**
 * Tests BvhNode::merge with 2 BB of different sizes at same position
 */
TEST_F(BvhTest, Merge_Case_2)
{
    
    IdGenerator idGenerator = IdGenerator();
    BvhNode *nodeLeft = new BvhNode(simpleBBList[2],idGenerator.GetUniqueId());
    BvhNode *nodeRight = new BvhNode(new AxisBoundingBox(nodeLeft->getBoundingBox().getCenter(), glm::vec3(2, 0.5, 4)),idGenerator.GetUniqueId());

    ///////////////////////////////////////
    BvhNode* actual = BvhNode::merge(nodeLeft, nodeRight,idGenerator.GetUniqueId());
    ///////////////////////////////////////

    glm::vec3 expectedCenter = glm::vec3{4, 0, 0};
    glm::vec3 expectedSize = glm::vec3{2, 1, 4};

    EXPECT_VEC3(actual->getBoundingBox().getCenter(), expectedCenter);
    EXPECT_VEC3(actual->getBoundingBox().getSize(), expectedSize);
}

/**
 * Tests BvhNode::merge with 2 BB of different sizes and different positions
 */
TEST_F(BvhTest, Merge_Case_3)
{
    IdGenerator idGenerator = IdGenerator();
    BvhNode *nodeLeft = new BvhNode(new AxisBoundingBox(glm::vec3{-1, 3, -7}, glm::vec3(2, 3, 5)),idGenerator.GetUniqueId());
    BvhNode *nodeRight = new BvhNode(new AxisBoundingBox(glm::vec3(-4, -5, 2), glm::vec3(3, 2, 1)),idGenerator.GetUniqueId());

    ///////////////////////////////////////
    BvhNode* actual = BvhNode::merge(nodeLeft, nodeRight,idGenerator.GetUniqueId());
    ///////////////////////////////////////

    glm::vec3 expectedCenter = glm::vec3{-2.5, -1, -2.5};
    glm::vec3 expectedSize = glm::vec3{5.5, 10.5, 12};

    EXPECT_VEC3(actual->getBoundingBox().getCenter(), expectedCenter);
    EXPECT_VEC3(actual->getBoundingBox().getSize(), expectedSize);
}

/**
 * Tests BvhTree::CreateMap with global case
 */
TEST_F(BvhTest, CreateMap_Case_1)
{
    IdGenerator idGenerator = IdGenerator();
    std::vector<BvhNode*> nodes;
    nodes.reserve(simpleBBList.size());
    for (auto bb : simpleBBList)
    {
        nodes.push_back(new BvhNode(bb,idGenerator.GetUniqueId()));
    }

    BvhTree tree;

    ///////////////////////////////////////
    tree.createMap(nodes);
    ///////////////////////////////////////

    std::multimap<float, BvhTree::PairNode> *actual = tree.getMap();
    std::vector<glm::vec3> centers2 = {
        {1, 0, 0},
        {-1, 0, 0},
        {4, 0, 0},
        {10, 0, 0}};
    std::vector<float> expected{2, 3, 5, 6, 9, 11};

    EXPECT_EQ(expected.size(), actual->size());

    int i = 0;
    for (auto it = actual->begin(); it != actual->end(); it++, i++)
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
    std::vector<BvhNode*> nodes = {new BvhNode(simpleBBList[0],idGenerator.GetUniqueId())};

    BvhTree tree;

    ///////////////////////////////////////
    tree.createMap(nodes);
    ///////////////////////////////////////

    std::multimap<float, BvhTree::PairNode> *actual = tree.getMap();
    std::vector<float> expected{};

    EXPECT_EQ(expected.size(), actual->size());
    EXPECT_EQ(expected.size(), 0);
}

/**
 * Tests BvhTree::CreateMap with 0 element
 */
TEST_F(BvhTest, CreateMap_Case_3)
{
    std::vector<BvhNode*> nodes;

    BvhTree tree;

    ///////////////////////////////////////
    tree.createMap(nodes);
    ///////////////////////////////////////

    std::multimap<float, BvhTree::PairNode> *actual = tree.getMap();
    std::vector<float> expected{};

    EXPECT_EQ(expected.size(), actual->size());
    EXPECT_EQ(expected.size(), 0);
}

/**
 * Tests BvhTree::CreateMap with 2 elements (bare minimum)
 */
TEST_F(BvhTest, CreateMap_Case_4)
{
    IdGenerator idGenerator = IdGenerator();
    std::vector<BvhNode*> nodes = {new BvhNode(simpleBBList[0],idGenerator.GetUniqueId()),new BvhNode(simpleBBList[1],idGenerator.GetUniqueId())};

    BvhTree tree;

    ///////////////////////////////////////
    tree.createMap(nodes);
    ///////////////////////////////////////

    std::multimap<float, BvhTree::PairNode> *actual = tree.getMap();
    std::vector<glm::vec3> centers2 = {
        {1, 0, 0},
        {-1, 0, 0}};
    std::vector<float> expected{2};

    EXPECT_EQ(expected.size(), actual->size());

    int i = 0;
    for (auto it = actual->begin(); it != actual->end(); it++, i++)
    {
        EXPECT_EQ(expected[i], it->first);
    }
}

TEST_F(BvhTest, Constructor_Case_1)
{

    std::cout << "BB done" << std::endl;
    BvhTree tree(bbs1);

    std::cout << "Hierarchisation terminee" << std::endl;
    tree.print();

    std::cout << "Print done" << std::endl;
    EXPECT_EQ(0, 0);
}
// Pour des BB même endroits mais taille différentes
TEST_F(BvhTest, Constructor_Case_2)
{
    std::vector<BoundingBox*> bbsSameCenter;
    glm::vec3 center = {1, 1, 1};
    std::vector<glm::vec3> sizes = {
        {6.5, 2.5, 3},
        {3.5, 2.5, 4},
        {3.5, 0.5, 2},
        {1.5, 1.5, 1},
        {3.5, 2.5, 7},
        {4.5, 5.5, 2.5},
        {1.5, 5.5, 3}};
    bbsSameCenter.reserve(sizes.size());
    for (auto const &value : sizes)
    {
        bbsSameCenter.push_back(new AxisBoundingBox(center, value));
    }
    std::cout << "BB done" << std::endl;
    BvhTree tree(bbsSameCenter);

    std::cout << "Hierarchisation terminee" << std::endl;
    tree.print();

    std::cout << "Print done" << std::endl;
    EXPECT_EQ(0, 0);
}

// Pour des BB a distances égale
TEST_F(BvhTest, Constructor_Case_3)
{
    std::vector<BoundingBox*> bbsSameDistance;
    glm::vec3 size = {1, 1, 1};
    std::vector<glm::vec3> centers;
    for (int i = 0;i<15;i++){
        centers.push_back(glm::vec3{i,i,i});
    }
    bbsSameDistance.reserve(centers.size());
    for (auto const &value : centers)
    {
        bbsSameDistance.push_back(new AxisBoundingBox(value, size));
    }
    std::cout << "BB done" << std::endl;
    BvhTree tree(bbsSameDistance);

    std::cout << "Hierarchisation terminee" << std::endl;
    tree.print();

    std::cout << "Print done" << std::endl;
    EXPECT_EQ(0, 0);
}
