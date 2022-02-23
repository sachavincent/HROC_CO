#include "utils/benchmark.hpp"
#include "engine.hpp"
#include "utils/ioutils.hpp"

#include <fstream>
#include <algorithm>

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
    _engine->getStaticCamera()->resetMovement();

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
        saveAsCsv();
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
                SceneBuilder::buildAsteroidField(engine, {50, 50, 90}, {0, 0, 45}, 300, 1.1, 0.1);
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
        engine->getStaticCamera()->updateDirection();
        engine->resetFrametime(); // reset frame time after scene buider hanging (fixes camera jump)
        engine->getStaticCamera()->moveZ(-moveSpeed*0.3);

        framecount++;
        return 0;
    }
    
    if (framecount < 1000)
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
    engine->getStaticCamera()->resetMovement();
    engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 2.0, 0.0}, 50.0, 90.0, 0.0});
    engine->getStaticCamera()->updateDirection();

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
                SceneBuilder::buildAsteroidField(engine, {50, 50, 100}, {0, 0, 50}, 600, 1.1, 0.1);
            engine->loadScene(scene);
            engine->getUi().setPipelineMode(true);
            results["asteroidMedium_ON"] = {};
            engine->getScene()->createBVH();
        }
        else
        {
            engine->getUi().setPipelineMode(false);
            results["asteroidMedium_OFF"] = {};
        }
        
        engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 0.0, 15.0}, 50.0, 90.0, 0.0});
        engine->getStaticCamera()->updateDirection();
        engine->resetFrametime(); // reset frame time after scene buider hanging (fixes camera jump)
        engine->getStaticCamera()->moveZ(-moveSpeed*0.5);

        framecount++;
        return 0;
    }
    if (framecount < 1000)
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
    engine->getStaticCamera()->resetMovement();
    engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 2.0, 0.0}, 50.0, 90.0, 0.0});
    engine->getStaticCamera()->updateDirection();
    
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
                SceneBuilder::buildAsteroidField(engine, {50, 50, 120}, {0, 0, 60}, 1000, 1.0, 0.1);
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
        engine->getStaticCamera()->updateDirection();
        engine->resetFrametime(); // reset frame time after scene buider hanging (fixes camera jump)
        engine->getStaticCamera()->moveZ(-moveSpeed*0.3);

        framecount++;
        return 0;
    }
    if (framecount < 1000)
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
    engine->getStaticCamera()->resetMovement();
    engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 2.0, 0.0}, 50.0, 90.0, 0.0});
    engine->getStaticCamera()->updateDirection();
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
        engine->getStaticCamera()->moveZ(moveSpeed);
        engine->resetFrametime(); 

        framecount++;
        return 0;
    }
    if (framecount < 1500)
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

    engine->getStaticCamera()->resetMovement();
    engine->getStaticCamera()->setCameraInfo({glm::vec3{0.0, 2.0, 0.0}, 50.0, 90.0, 0.0});
    engine->getStaticCamera()->updateDirection();
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

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void Benchmark::saveAsCsv() {

    for(auto const& result: results){
        std::fstream fs;
        
        std::string fname = Utils::workingDirectory()+"benchmarks\\"+result.first+".csv";
        fname = ReplaceAll( fname, "\\", "/");
        

        fs.open(fname.c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
        fs << "frameNum" <<";"<< "delaTime" << "\r";
        for(int i = 0; i<result.second.size();i++){
            fs << i <<";"<<result.second[i] << "\r";
        }
        std::cout << fname;
        fs.close();
    }
}