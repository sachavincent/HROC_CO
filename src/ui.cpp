#include "ui.hpp"
#include "engine.hpp"
#include "scene.hpp"

#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

Ui::Ui() {}

void Ui::load(GLFWwindow *_window, Engine *_engine)
{
    engine = _engine;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void Ui::render()
{
    Scene &scene = engine->getScene();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    //TODO: test implot
    ImPlot::CreateContext();
    int   bar_data[8] = {1,3,1,3,5,2,7,3};
    if (ImPlot::BeginPlot("My Plot")) {
        ImPlot::PlotBars("My Bar Plot", bar_data, 8);
        ImPlot::EndPlot();
    }


    ImGui::Begin("Parameters");
    static float camspeed = scene.getCamera()->getMoveSpeed();
    ImGui::DragFloat("camera movement speed", &camspeed, 0.1, 0.1, 40.0);
    scene.getCamera()->setMoveSpeed(camspeed);

    float exposure = scene.getExposure();
    ImGui::DragFloat("exposure", &exposure, 0.01, 0.01, 10.0);
    scene.setExposure(exposure);

    ImGui::Checkbox("Display Objects", &objectMode);

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
            bboxMode = dispBboxLevel;
        }
        else
        {
            // display all
            bboxMode = 0;
        }
    }
    else
    {
        bboxMode = -1;
    }

    ImGui::Separator();

    sceneParams();

    lightsParams();
    objectsParams();
    if (newLightWindowActive) newLightWindow();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Ui::lightsParams()
{
    if (ImGui::TreeNode("Lights Parameters"))
    {

        // display the lights to choose from
        const char *items[100];
        std::vector<std::shared_ptr<Light>> &lights = engine->getScene().getLights();
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

void Ui::objectsParams()
{
    if (ImGui::TreeNode("Objects Parameters"))
    {
        const char *items[200];
        auto &objects = engine->getScene().getObjects();
        std::vector<std::string> objectsNames;
        for (int i = 0; i < 200; i++)
            objectsNames.push_back(objects[i]->getName());

        for (int i = 0; i < 200; i++)
        {
            items[i] = objectsNames[i].c_str();
        }

        ImGui::ListBox("Select a object", &objectsListIndex, items, objectsNames.size(), 5);
        auto object = objects[objectsListIndex];

        std::string objectText = "\n" + object->getName() + " parameters:\n";
        ImGui::Text(objectText.c_str());
        // position
        glm::vec3 pos = object->getPosition();
        ImGui::DragFloat3("Position", &pos[0], 0.01, -10.0, 10.0);
        object->setPosition(pos);
        // position
        glm::vec3 scale = object->getScale();
        ImGui::DragFloat3("Scale", &scale[0], 0.01, 0.01, 10.0);
        object->setScale(scale);

        glm::vec3 diffuse = object->getDiffuse();
        ImGui::ColorEdit3("Diffuse", &diffuse[0]);
        object->setDiffuse(diffuse);
        glm::vec3 specular = object->getSpecular();
        ImGui::ColorEdit3("Specular", &specular[0]);
        object->setSpecular(specular);
        float shininess = object->getShininess();
        ImGui::DragFloat("Shininess", &shininess, 1.0, 5.0, 256.0);
        object->setShininess(shininess);

        ImGui::Text("Texture Parameters:");
        if (object->hasTextures())
        {
            glm::vec2 texScale = object->getTexScaling();
            ImGui::DragFloat2("Texture Scaling", &texScale[0], 0.1, 0.1, 20.0);
            object->setTexScaling(texScale);
        }

        ImGui::TreePop();
    }
}

void Ui::sceneParams()
{

    if (ImGui::TreeNode("Scene Loading"))
    {
        static const char *items[2] = {"asteroid", "Scene from Obj"};
        static int sceneTypeIndex = 0;
        static int numAst = 0;


            ImGui::ListBox("Select a Type of Scene", &sceneTypeIndex, items, 2, 2);

        switch (sceneTypeIndex)
        {
        case 0:
            //ImGui::InputInt("Number of asteroids",&numAst,1000,100000);
            break;
        case 1:
            //ImGui::InputStr("Number of asteroids",&numAst,1000,100000);
            break;
        }
        if (ImGui::Button("Load Scene"))
        {
            switch (sceneTypeIndex)
            {
            case 0:
                //Scene *scene = SceneBuilder::buildAsteroidField(
                //    engine, glm::vec3{40, 40, 150}, glm::vec3{0.0f, 0.0f, 70.0f}, 2000, 0.8, 0.5);
                //engine->loadScene(scene);
                //break;
            case 1:

                break;
            }
        }
        // TODO: following procedure should be automated in gui
        // delete previous -> build a scene -> load it to engine -> generate BVH -> repeat
        // delete testScene;
        // testScene =  SceneBuilder::buildAsteroidField(&engine, glm::vec3{40,40,150},
        // glm::vec3{0.0f,0.0f,70.0f}, 2000, 0.8, 0.5); engine.loadScene(testScene);
        // testScene->createBVH();

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
        engine->getScene().addLight(pl);
        newLightWindowActive = false;
    }

    ImGui::End();
}