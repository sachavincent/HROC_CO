#include "gtest/gtest.h"

#include "camera.hpp"
#include "frustum.hpp"



class VfcTest : public ::testing::Test
{
protected:
    VfcTest()
    {
        // You can do set-up work for each test here.


    }
    virtual ~VfcTest()
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

};

/**
 * Tests BvhNode::merge with 2 BB of same size at different positions
 */
TEST_F(VfcTest, Vfc_Case_1)
{

    Camera camera(1920,1080,glm::vec3(0,0,0),90);

    std::shared_ptr<BoundingBox> bb1 
        = std::shared_ptr<BoundingBox>(new AxisBoundingBox(glm::vec3(0,0,5),glm::vec3(2,2,2)));

    
    std::shared_ptr<BoundingBox> bb2
        = std::shared_ptr<BoundingBox>(new AxisBoundingBox(glm::vec3(100,0,0),glm::vec3(10,10,10)));

    
    std::shared_ptr<BoundingBox> bb3
        = std::shared_ptr<BoundingBox>(new AxisBoundingBox(glm::vec3(0,0,1000000),glm::vec3(2,2,2)));


    std::shared_ptr<BoundingBox> bb4
        = std::shared_ptr<BoundingBox>(new AxisBoundingBox(glm::vec3(0,0,-5),glm::vec3(2,2,2)));

    EXPECT_EQ(camera.getFrustum()->isInFrustum(bb1),true);
    EXPECT_EQ(camera.getFrustum()->isInFrustum(bb2),false);
    EXPECT_EQ(camera.getFrustum()->isInFrustum(bb3),false);
    EXPECT_EQ(camera.getFrustum()->isInFrustum(bb4),false);



}
