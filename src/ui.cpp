#include "ui.hpp"

#include "scene.hpp"

#include <memory>
#include <type_traits>
#include <mutex>
#include <utility>

Ui::Ui() {}

void Ui::load(GLFWwindow *_window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}
void Ui::render(Scene *_scene)
{
    scene = _scene;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Scene parameters");

    float exposure = scene->getExposure();
    ImGui::DragFloat("exposure", &exposure, 0.01, 0.01, 10.0);
    scene->setExposure(exposure);
    ImGui::Separator();

    static bool dispBbox = false;
    static bool dispSpecificBbox = false;
    static int dispBboxLevel = 1;

    ImGui::Checkbox("Display Objects", &objectMode);
    ImGui::Separator();
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

    lightsParams();
    objectsParams();
    if (newLightWindowActive)
        newLightWindow();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Ui::lightsParams()
{
    if (ImGui::TreeNode("Lights Parameters"))
    {

        // display the light of light to choose from
        const char *items[100];
        std::vector<std::shared_ptr<Light>> &lights = scene->getLights();
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
            nlMult = 1.0;
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
        std::call_once(flag1, [&]()
                       {
            for(size_t i = 0; i<lights.size(); i++){
                difColor[i] = lights[i]->getColor();
            } });

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
        auto &objects = scene->getObjects();
        std::vector<std::string> objectsNames;
        for (auto &mo : objects)
            objectsNames.push_back(mo->getName());

        for (int i = 0; i < objectsNames.size(); i++)
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

void Ui::newLightWindow()
{
    ImGui::Begin("Add a light");
    const char *items[] = {"Point Light", "Distant Light"};
    static int item_current = 0;
    ImGui::Combo("Light Type", &item_current, items, IM_ARRAYSIZE(items));

    switch (item_current)
    {
    case 0:
        newPointLight();
        break;
    case 1:
        newDistantLight();
        break;
    default:
        break;
    }
    ImGui::End();
}

void Ui::newPointLight()
{
    static glm::vec3 nlPos;
    static glm::vec3 nlColor;

    ImGui::DragFloat3("position", &nlPos[0], 0.01, -10.0, 10.0);
    ImGui::DragFloat("Light Power Multiplier", &nlMult, 0.1, 0.1, 100.0);
    ImGui::ColorEdit3("Color", &nlColor[0]);

    if (ImGui::Button("Add"))
    {
        auto pl = std::make_shared<Light>(nlPos, nlColor * nlMult);
        nlMult = 1.0;
        scene->addLight(pl);
        newLightWindowActive = false;
    }
}

void Ui::newDistantLight()
{
    static glm::vec3 nlPos;
    static glm::vec3 nlColor;

    ImGui::DragFloat3("Incoming Position", &nlPos[0], 0.01, -1.0, 1.0);
    ImGui::DragFloat("Light Power Multiplier", &nlMult, 0.1, 0.0, 100.0);
    ImGui::ColorEdit3("Color", &nlColor[0]);

    if (ImGui::Button("Add"))
    {
        auto dl = std::make_shared<Light>(nlPos, nlColor * nlMult);
        scene->addLight(dl);
        newLightWindowActive = false;
    }
}