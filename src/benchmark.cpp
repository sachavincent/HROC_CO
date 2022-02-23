#include "utils/benchmark.hpp"
#include "engine.hpp"
#include "utils/ioutils.hpp"

Benchmark::Benchmark(Engine *_engine, BenchFlags _flags) : engine(_engine)
{

    GLFWwindow *win = _engine->getWindow();
    // disabe mouse buttons and Gui
    glfwSetCursorPosCallback(win, nullptr);
    glfwSetKeyCallback(win, nullptr);
    glfwSetScrollCallback(win, nullptr);
    glfwSetFramebufferSizeCallback(win, nullptr);
    glfwSetWindowSizeCallback(win, nullptr);
    _engine->disableGui();
    _engine->setCameraType(CameraType::STATIC);
    _engine->getStaticCamera()->setConstantSpeed(true);

    // setup benchmark steps
    if ((_flags & BenchFlags_Meteorite) == BenchFlags_Meteorite)
    {
        if ((_flags & BenchFlags_LowTests) == BenchFlags_LowTests)
            benchFuns.push_back(std::bind(&Benchmark::BNCH_asteroidLow, this));
        if ((_flags & BenchFlags_MediumTests) == BenchFlags_MediumTests)
            benchFuns.push_back(std::bind(&Benchmark::BNCH_asteroidMedium, this));
        if ((_flags & BenchFlags_HighTests) == BenchFlags_HighTests)
            benchFuns.push_back(std::bind(&Benchmark::BNCH_asteroidHigh, this));
    }

    if ((_flags & BenchFlags_City) == BenchFlags_City)
    {
        if ((_flags & BenchFlags_LowTests) == BenchFlags_LowTests)
            benchFuns.push_back(std::bind(&Benchmark::BNCH_cityLow, this));
        if ((_flags & BenchFlags_MediumTests) == BenchFlags_MediumTests)
            benchFuns.push_back(std::bind(&Benchmark::BNCH_cityMedium, this));
        if ((_flags & BenchFlags_HighTests) == BenchFlags_HighTests)
            benchFuns.push_back(std::bind(&Benchmark::BNCH_cityHigh, this));
    }
}

void Benchmark::exec()
{
    // bench finished
    if (benchStep == benchFuns.size())
    {
        if (saveResults) saveAsCsv();
        finishedSignal = true;

        GLFWwindow *win = engine->getWindow();
        engine->enableGui();
        engine->getStaticCamera()->setConstantSpeed(false);
        engine->setCameraType(CameraType::FREE);

        glfwSetCursorPosCallback(win, IOUtils::mouseCallback);
        glfwSetKeyCallback(win, IOUtils::keyCallback);
        glfwSetScrollCallback(win, IOUtils::scrollCallback);
        glfwSetFramebufferSizeCallback(win, IOUtils::framebufferSizeCallback);
        glfwSetWindowSizeCallback(win, IOUtils::updateScreenRes);
        return;
    }
    // execute the benchmark step
    bool done = benchFuns[benchStep]();
    // if step finished
    if (done)
    {
        benchStep++;
    }
}

bool Benchmark::BNCH_asteroidLow()
{
    static int framecount = 0;
    static int passNum = 0;
    if (framecount == 0)
    {

        if (passNum == 0)
        {
            delete engine->getScene();
            Scene *scene =
                SceneBuilder::buildAsteroidField(engine, {40, 40, 100}, {0, 0, 70}, 100, 1.0, 0.1);
            engine->loadScene(scene);
            engine->getUi().setPipelineMode(true);
            results["asteroidLow_ON"] = {};
            engine->getScene()->createBVH();
        }
        else
        {
            engine->getUi().setPipelineMode(false);
            results["asteroidLow_OFF"] = {};
        }
        
        engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 0.0, 15.0}, 50.0, 90.0, 0.0});
        engine->resetFrametime(); // reset frame time after scene buider hanging (fixes camera jump)
        engine->getStaticCamera()->moveZ(-moveSpeed);

        framecount++;
        return 0;
    }
    if (framecount < 500)
    {
        if (passNum == 0)
        {
            results["asteroidLow_ON"].push_back(engine->getDeltaTime());
        }
        else
        {
            results["asteroidLow_OFF"].push_back(engine->getDeltaTime());
        }
        framecount++;
        return 0;
    }
    engine->getStaticCamera()->moveZ(moveSpeed);
    engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 0.0, 15.0}, 50.0, 90.0, 0.0});
    framecount = 0;
    if (passNum == 0)
    {
        passNum++;
        return 0;
    }
    passNum = 0;
    return 1;
}

bool Benchmark::BNCH_asteroidMedium()
{
    static int framecount = 0;
    static int passNum = 0;
    if (framecount == 0)
    {

        if (passNum == 0)
        {
            delete engine->getScene();
            Scene *scene =
                SceneBuilder::buildAsteroidField(engine, {40, 40, 100}, {0, 0, 70}, 300, 1.0, 0.1);
            engine->loadScene(scene);
            engine->getUi().setPipelineMode(true);
            results["asteroidLMedium_ON"] = {};
            engine->getScene()->createBVH();
        }
        else
        {
            engine->getUi().setPipelineMode(false);
            results["asteroidMedium_OFF"] = {};
        }
        
        engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 0.0, 15.0}, 50.0, 90.0, 0.0});
        engine->resetFrametime(); // reset frame time after scene buider hanging (fixes camera jump)
        engine->getStaticCamera()->moveZ(-moveSpeed);

        framecount++;
        return 0;
    }
    if (framecount < 500)
    {
        if (passNum == 0)
        {
            results["asteroidMedium_ON"].push_back(engine->getDeltaTime());
        }
        else
        {
            results["asteroidMedium_OFF"].push_back(engine->getDeltaTime());
        }
        framecount++;
        return 0;
    }
    engine->getStaticCamera()->moveZ(moveSpeed);
    engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 0.0, 15.0}, 50.0, 90.0, 0.0});
    framecount = 0;
    if (passNum == 0)
    {
        passNum++;
        return 0;
    }
    passNum = 0;
    return 1;
}

bool Benchmark::BNCH_asteroidHigh()
{
    static int framecount = 0;
    static int passNum = 0;
    if (framecount == 0)
    {
        if (passNum == 0)
        {
            delete engine->getScene();
            Scene *scene =
                SceneBuilder::buildAsteroidField(engine, {40, 40, 100}, {0, 0, 70}, 500, 1.0, 0.1);
            engine->loadScene(scene);
            engine->getUi().setPipelineMode(true);
            results["asteroidHigh_ON"] = {};
            engine->getScene()->createBVH();
        }
        else
        {
            engine->getUi().setPipelineMode(false);
            results["asteroidHigh_OFF"] = {};
        }
        
        engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 0.0, 15.0}, 50.0, 90.0, 0.0});
        engine->resetFrametime(); // reset frame time after scene buider hanging (fixes camera jump)
        engine->getStaticCamera()->moveZ(-moveSpeed);

        framecount++;
        return 0;
    }
    if (framecount < 500)
    {
        if (passNum == 0)
        {
            results["asteroidHigh_ON"].push_back(engine->getDeltaTime());
        }
        else
        {
            results["asteroidHigh_OFF"].push_back(engine->getDeltaTime());
        }
        framecount++;
        return 0;
    }
    engine->getStaticCamera()->moveZ(moveSpeed);
    engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 0.0, 15.0}, 50.0, 90.0, 0.0});
    framecount = 0;
    if (passNum == 0)
    {
        passNum++;
        return 0;
    }
    passNum = 0;
    return 1;
}

bool Benchmark::BNCH_cityLow()
{
    static int framecount = 0;
    static int passNum = 0;
    if (framecount == 0)
    {
        if (passNum == 0)
        {
            delete engine->getScene();
            Scene *scene = SceneBuilder::buildMultiMesh(engine, "models/paul_sab/paul_sab_527.obj");
            engine->loadScene(scene);
            engine->getUi().setPipelineMode(true);
            results["cityLow_ON"] = {};
            engine->getScene()->createBVH();
        }
        else
        {
            engine->getUi().setPipelineMode(false);
            results["cityLow_OFF"] = {};
        }
        engine->getStaticCamera()->setCameraInfo({glm::vec3{14.6, 8.0, -285.0}, 60.0, 60.0, 0.0});
        engine->getStaticCamera()->updateDirection();
        engine->getStaticCamera()->moveZ(moveSpeed * 1.88);
        //engine->getStaticCamera()->moveZ(moveSpeed * 0.01);
        engine->resetFrametime(); // reset frame time after scene buider hanging (fixes camera jump)

        framecount++;
        return 0;
    }
    if (framecount < 500)
    {
        //std::cout << glm::to_string(engine->getStaticCamera()->getPosition()) << std::endl;
        if (passNum == 0)
        {
            results["cityLow_ON"].push_back(engine->getDeltaTime());
        }
        else
        {
            results["cityLow_OFF"].push_back(engine->getDeltaTime());
        }
        framecount++;
        return 0;
    }
    engine->getStaticCamera()->moveZ(-moveSpeed * 1.88);
    //engine->getStaticCamera()->moveZ(-moveSpeed * 0.01);
    engine->getStaticCamera()->setCameraInfo({glm::vec3{14.6, 8.0, -285.0}, 60.0, 60.0, 0.0});
    framecount = 0;
    if (passNum == 0)
    {
        passNum++;
        return 0;
    }
    passNum = 0;
    return 1;
}

bool Benchmark::BNCH_cityMedium() { return 1; }

bool Benchmark::BNCH_cityHigh() { return 1; }

void Benchmark::saveAsCsv() {}