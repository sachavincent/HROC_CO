#include "ui.hpp"

#include "scene.hpp"

#include <memory>
#include <type_traits>

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

        //display the light of light to choose from
        const char *items[100];
        std::vector<Light *> &lights = scene->getLights();
        std::vector<std::string> lightsNames;
        for (Light *li : lights)
            lightsNames.push_back("Light");

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
        Light *light = lights[lightListIndex];

        std::string lightText = "\nLight parameters:\n";
        ImGui::Text(lightText.c_str());
        // Change parameters for selected light

        //diffuse color
        static float staticmult;
        if (lightListIndex != lastlightListIndex)
            staticmultSet = false;
        if (!staticmultSet)
        {
            staticmult = 1.0;
            staticmultSet = true;
        }
        ImGui::DragFloat("Color Multiplier", &staticmult, 0.1, 0.01, 20.0);
        glm::vec3 diffuse = light->getColor() / staticmult;
        ImGui::ColorEdit3("Color", &diffuse[0]);
        light->setColor(diffuse * staticmult);

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
        std::vector<Model *> &models = scene->getModels();
        std::vector<std::string> modelsNames;
        for (auto *mo : models)
            modelsNames.push_back(mo->getName());

        for (int i = 0; i < modelsNames.size(); i++)
        {
            items[i] = modelsNames[i].c_str();
        }

        ImGui::ListBox("Select a Model", &modelListIndex, items, modelsNames.size(), 5);
        Model *model = models[modelListIndex];

        std::string modelText = "\n" + model->getName() + " parameters:\n";
        ImGui::Text(modelText.c_str());
        // position
        glm::vec3 pos = model->getPosition();
        ImGui::DragFloat3("Position", &pos[0], 0.01, -10.0, 10.0);
        model->setPosition(pos);
        // position
        glm::vec3 scale = model->getScale();
        ImGui::DragFloat3("Scale", &scale[0], 0.01, 0.01, 10.0);
        model->setScale(scale);

        glm::vec3 diffuse = model->getDiffuse();
        ImGui::ColorEdit3("Diffuse", &diffuse[0]);
        model->setDiffuse(diffuse);
        glm::vec3 specular = model->getSpecular();
        ImGui::ColorEdit3("Specular", &specular[0]);
        model->setSpecular(specular);
        float shininess = model->getShininess();
        ImGui::DragFloat("Shininess", &shininess, 1.0, 5.0, 256.0);
        model->setShininess(shininess);

        ImGui::Text("Texture Parameters:");
        if (model->hasTextures())
        {
            glm::vec2 texScale = model->getTexScaling();
            ImGui::DragFloat2("Texture Scaling", &texScale[0], 0.1, 0.1, 20.0);
            model->setTexScaling(texScale);
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
        Light *pl = new Light(nlPos, nlColor * nlMult);
        nlMult = 1.0;
        scene->addLight(*pl);
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
        Light *dl = new Light{nlPos, nlColor * nlMult};

        scene->addLight(*dl);
        newLightWindowActive = false;
    }
}