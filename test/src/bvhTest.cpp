#include "gtest/gtest.h"
#include "bvhtree.hpp"

class BvhTest : public ::testing::Test
{
protected:
    BvhTest() : tree(nullptr)
    {
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
    BvhTree *tree;
};

TEST_F(BvhTest, Constructor_Case_1)
{
    BvhTree tree;
    EXPECT_EQ(0, f.Bar(input_filepath, output_filepath));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}