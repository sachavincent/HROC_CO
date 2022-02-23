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
 * Tests Vfc::test with many boxes 
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
            std::shared_ptr<BoundingBox> bb5
        = std::shared_ptr<BoundingBox>(new AxisBoundingBox(glm::vec3(10,10,10),glm::vec3(2,2,2)));

    const Frustum* frustum =  camera.getFrustum();

    EXPECT_EQ(frustum->isInFrustum(bb1),false);
    EXPECT_EQ(frustum->isInFrustum(bb2),false);
    EXPECT_EQ(frustum->isInFrustum(bb3),false);
    EXPECT_EQ(frustum->isInFrustum(bb4),false);
    EXPECT_EQ(frustum->isInFrustum(bb5),true);
}


/**
 * Tests Vfc::check computation for vfc
 */
TEST_F(VfcTest, Vfc_Case_2)
{

    Camera camera(1920,1080,glm::vec3(0,0,0),90);

    std::shared_ptr<BoundingBox> bb1 
        = std::shared_ptr<BoundingBox>(new AxisBoundingBox(glm::vec3(0,0,5),glm::vec3(2,2,2)));

    
    std::shared_ptr<BoundingBox> bb2
        = std::shared_ptr<BoundingBox>(new AxisBoundingBox(glm::vec3(100,0,0),glm::vec3(10,10,10)));

    
    std::shared_ptr<BoundingBox> bb3
        = std::shared_ptr<BoundingBox>(new AxisBoundingBox(glm::vec3(0,0,10000),glm::vec3(2,2,2)));


    std::shared_ptr<BoundingBox> bb4
        = std::shared_ptr<BoundingBox>(new AxisBoundingBox(glm::vec3(0,0,-5),glm::vec3(2,2,2)));

    const Frustum* frustum =  camera.getFrustum();
    printf(glm::to_string(frustum->topFace.normal).c_str());

    Plan plan1(glm::vec3(0,3,0),glm::vec3(0,1,0));
    Plan plan2(glm::vec3(80,0,0),glm::vec3(1,0,0));


    //BB1 in (0,0,5) plan in (0,3,0) with normal (0,1,0)   
    EXPECT_EQ(bb1->isOnOrForwardPlan(plan1),false);

    //BB2 in (100,0,0) plan in (100,0,0) with any normal  
    plan2.normal = glm::vec3(1,0,0);
    EXPECT_EQ(bb2->isOnOrForwardPlan(plan2),true);
    plan2.normal = glm::vec3(0,1,0);
    EXPECT_EQ(bb2->isOnOrForwardPlan(plan2),false);
    plan2.normal = glm::vec3(0,0,1);
    EXPECT_EQ(bb2->isOnOrForwardPlan(plan2),false);
}