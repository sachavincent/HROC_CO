#include "ui.hpp"
#include "engine.hpp"
#include "scene.hpp"
//#include "utils/benchmark.hpp"

#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

#include <numeric>

Ui::Ui() {}

void Ui::load(GLFWwindow *_window, Engine *_engine)
{
    engine = _engine;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void Ui::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Scene Parameters");
    displayParams();
    ImGui::Separator();
    sceneLoading();
    ImGui::Separator();
    lightsParams();
    ImGui::End();

    ImGui::Begin("Performance & Benchmark");
    plotTimer();
    ImGui::Separator();
    plotFpsRate();
    ImGui::End();

    ImGui::Begin("Pipeline options ");
    displayPipelineOptions();
    ImGui::End();

    if (newLightWindowActive)
        newLightWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//! Parameters for general display/rendering options
void Ui::displayParams()
{
    Scene *scene = engine->getScene();
    static float camspeed = scene->getCamera()->getMoveSpeed();
    std::string cameraType;
    engine->getCurrentCameraType() == CameraType::FREE ? cameraType = "Free Cam"
                                                       : cameraType = "Static Cam";
    ImGui::Text(cameraType.c_str());
    ImGui::Separator();
    ImGui::DragFloat("camera movement speed", &camspeed, 0.1, 0.1, 40.0);
    scene->getCamera()->setMoveSpeed(camspeed);

    float exposure = scene->getExposure();
    ImGui::DragFloat("exposure", &exposure, 0.01, 0.01, 10.0);
    scene->setExposure(exposure);

    ImGui::Checkbox("Display Objects", &objectMode);

    ImGui::Separator();
    ImGui::Checkbox("Display Frustum Outline", &frustumOutlineMode);
    ImGui::Separator();
    ImGui::Checkbox("Display Frustum", &frustumMode);

    ImGui::Separator();
    static bool dispBbox = false;
    static bool dispSpecificBbox = false;
    static int dispBboxLevel = 1;
    ImGui::Checkbox("Display Bounding Boxes", &dispBbox);

    if (dispBbox)
    {
        ImGui::Checkbox("Display only for specific BVH level", &dispSpecificBbox);
        if (dispSpecificBbox)
        {
            ImGui::SliderInt("Level to Display", &dispBboxLevel, 1, bboxMaxLevel);
            bboxMode = dispBboxLevel; // level num
        }
        else
        {
            bboxMode = 0; // all
        }
    }
    else
    {
        bboxMode = -1; // none
    }
    ImGui::Separator();
    ImGui::Checkbox("Display occludees/occluders", &occludeeColorMode);
}

//! Parameters for lights in the scene
void Ui::lightsParams()
{
    if (ImGui::TreeNode("Lights Parameters"))
    {
        // display the lights to choose from
        const char *items[100];
        std::vector<std::shared_ptr<Light>> &lights = engine->getScene()->getLights();
        std::vector<std::string> lightsNames;

        for (size_t i = 0; i < lights.size(); i++)
            lightsNames.push_back("Light_" + std::to_string(i));

        for (int i = 0; i < lightsNames.size(); i++)
        {
            items[i] = lightsNames[i].c_str();
        }
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(64, 64, 255, 255));
        if (ImGui::Button("Add a Light") && lights.size() < 100)
        {
            newLightWindowActive = true;
        }
        ImGui::PopStyleColor();
        ImGui::ListBox("Select a Light to modify", &lightListIndex, items, lightsNames.size(), 5);
        auto light = lights[lightListIndex];

        // Change parameters for selected light
        std::string lightText = "\nLight parameters:\n";
        ImGui::Text(lightText.c_str());

        // diffuse color
        static std::vector<float> staticmult(100, 1.0f);
        static std::vector<glm::vec3> difColor(100, glm::vec3{1.0f});
        static std::once_flag flag1;
        std::call_once(flag1,
                       [&]()
                       {
                           for (size_t i = 0; i < lights.size(); i++)
                           {
                               difColor[i] = lights[i]->getColor();
                           }
                       });

        ImGui::DragFloat("Color Multiplier", &staticmult[lightListIndex], 0.5, 0.1, 100.0);
        ImGui::ColorEdit3("Color", &difColor[lightListIndex][0]);

        light->setColor(difColor[lightListIndex] * staticmult[lightListIndex]);

        // change pos
        glm::vec3 pos = light->getPosition();
        ImGui::DragFloat3("Position", &pos[0], 0.1f, -10.0f, 10.0f);

        light->setPosition({pos[0], pos[1], pos[2]});

        // delete the light
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 30, 30, 255));
        std::string deltext = "Delete light";
        if (ImGui::Button(deltext.c_str()) && lights.size() > 1)
        {
            if (lightListIndex < lights.size() - 1)
            {
                lights.erase(lights.begin() + lightListIndex);
            }
            else
            {
                lights.pop_back();
                lightListIndex -= 1;
            }
        }
        ImGui::PopStyleColor();
        ImGui::TreePop();
    }
    lastlightListIndex = lightListIndex;
}

//! Scene loading interface
void Ui::sceneLoading()
{
    if (ImGui::TreeNode("Scene Loading"))
    {
        static const char *items[3] = {"Base Scene", "Asteroid Scene", "Scene from .obj"};
        static int sceneTypeIndex = 0;

        static int numAst = 20;
        static float astSize = 1.0f;
        static float astScaleRatio = 0.2f;
        static glm::vec3 domainSize = {30, 30, 100};
        static glm::vec3 domainCenter = {0, 0, 50};
        static bool buildBvh = true;
        static std::string mmesh_path = "";

        ImGui::Combo("Select a Type of Scene", &sceneTypeIndex, items, 3);

        switch (sceneTypeIndex)
        {
        case 0:
            break;
        case 1:
        {
            ImGui::DragInt("Number of asteroids", &numAst, 10, 1, 1000000);
            ImGui::DragFloat3("Domain size", &domainSize[0], 1.0, 1.0, 10000.0);
            ImGui::DragFloat3("Domain center", &domainCenter[0], 1.0, 1.0, 10000.0);
            ImGui::DragFloat("Asteroid Base Size", &astSize, 0.01, 0.1, 100.0);
            ImGui::DragFloat("Random Scale Factor ]0,1[", &astScaleRatio, 0.01, 0.001, 0.999);
        }
        break;
        case 2:
            if (ImGui::Button("Choose File"))
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".obj",
                                                        ".");

            // display
            if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    mmesh_path = ImGuiFileDialog::Instance()->GetFilePathName();
                }
                ImGuiFileDialog::Instance()->Close();
            }
            ImGui::Text((std::string{"Current file:\n"} + mmesh_path).c_str());
            break;
        }

        ImGui::Checkbox("Build Bvh Tree", &buildBvh);
        if (ImGui::Button("Load Scene"))
        {
            Scene *scene = nullptr;
            delete engine->getScene();
            switch (sceneTypeIndex)
            {
            case 0:
            {
                scene = SceneBuilder::buildDefaultScene(engine);
            }
            break;
            case 1:
            {
                scene = SceneBuilder::buildAsteroidField(engine, domainSize, domainCenter, numAst,
                                                         astSize, astScaleRatio);
            }
            break;
            case 2:
                scene = SceneBuilder::buildMultiMesh(engine, mmesh_path);
                break;
            }
            engine->loadScene(scene);
            if (buildBvh)
            {
                scene->createBVH();
            }
        }
        ImGui::TreePop();
    }
}

void Ui::newLightWindow()
{
    ImGui::Begin("Add a light");

    static glm::vec3 nlPos = glm::vec3{0.0};
    static glm::vec3 nlColor = glm::vec3{1.0};
    static glm::vec3 attn = {0, 0, 1};
    static float mult = 1.0;

    ImGui::DragFloat3("position", &nlPos[0], 0.01, -10.0, 10.0);
    ImGui::DragFloat("Light Power Multiplier", &mult, 0.1, 0.1, 100.0);
    ImGui::ColorEdit3("Color", &nlColor[0]);
    ImGui::DragFloat3("Attenuation (const/lin/quad)", &attn[0], 0.001, 0.1, 2.0);

    if (ImGui::Button("Add"))
    {
        auto pl = std::make_shared<Light>(nlPos, nlColor * mult, attn);
        mult = 1.0;
        engine->getScene()->addLight(pl);
        newLightWindowActive = false;
    }

    ImGui::End();
}

void Ui::plotTimer()
{
    Scene *scene = engine->getScene();
    double timers[9];
    for (int i = 0; i < 7; i++)
    {
        timers[i] = round(scene->timers[i] * 1000);
        if (timers[i] < 0)
        {
            timers[i] = 0.0;
        }
    }

    static ImPlotAxisFlags xflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;
    static ImPlotAxisFlags yflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;
    if (ImPlot::BeginPlot("Pipeline Performance", NULL, NULL, ImVec2(350, 350)))
    {
        ImPlot::SetupAxes("", "", xflags, yflags);
        ImPlot::PlotPieChart(scene->timerLabels, timers, 7, 0.5f, 0.5f, 0.4f);
        ImPlot::EndPlot();
    }
}

void Ui::plotFpsRate()
{
    static std::vector<double> fpsVec(80, 0.0);
    static std::vector<double> fpsVecAvg30(80, 0.0);

    static double refreshCounter = 0.0;
    static float updateSpeedMs = 100.0f;

    if (refreshCounter > updateSpeedMs / 1000)
    {
        for (size_t i = 0; i < 79; i++)
        {
            fpsVec[i] = fpsVec[i + 1];
        }

        for (size_t i = 30; i < 80; i++)
        {
            fpsVecAvg30[i] = std::reduce(&fpsVec[i - 30], &fpsVec[i]) / 30;
        }

        fpsVec[79] = 1 / engine->deltaTime;
        refreshCounter = 0.0;
    }
    refreshCounter += engine->deltaTime;

    static ImPlotAxisFlags xflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

    static ImPlotAxisFlags yflags = ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_LockMin;

    static ImPlotLegendFlags legendFlags = ImPlotLegendFlags_None;

    ImGui::SliderFloat("update speed (ms)", &updateSpeedMs, 5.0f, 200.0f);
    if (ImPlot::BeginPlot("Fps Curves", NULL, NULL, ImVec2(350, 200), 0))
    {

        ImPlot::SetupAxes("time", "FPS", xflags, yflags);
        ImPlot::SetupLegend(ImPlotLocation_SouthWest, legendFlags);
        std::vector<double> instantCrop(fpsVec.begin() + 30, fpsVec.end());
        ImPlot::PlotLine("instant fps", instantCrop.data(), 50);

        std::vector<double> avgCrop(fpsVecAvg30.begin() + 30, fpsVecAvg30.end());
        ImPlot::PlotLine("moving average 30", avgCrop.data(), 50);

        ImPlot::EndPlot();
    }
}

void Ui::displayPipelineOptions()
{
    ImGui::Checkbox("First Early Z", &firstEarlyZMode);
    ImGui::Separator();

    ImGui::Checkbox("Extract Occludees", &extractOccludeesMode);
    ImGui::Separator();

    ImGui::Checkbox("View Frustum Culling", &viewFrustumCullingMode);
    ImGui::Separator();

    ImGui::Checkbox("Batch Occlusion Test", &batchOcclusionMode);
    ImGui::Separator();
    
    ImGui::Checkbox("Second Early Z", &secondEarlyZMode);
    ImGui::Separator();
}