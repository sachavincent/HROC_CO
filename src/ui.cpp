#include "ui.hpp"

#include "scene.hpp"

#include <memory>
#include <type_traits>

Ui::Ui(){}

void Ui::load(GLFWwindow* _window){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}
void Ui::render(Scene* _scene){
    scene = _scene;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Scene parameters");
    bool oldSSAOstatus = SSAOstatus;
    ImGui::Checkbox("SSAO Technique",&SSAOstatus);
    if(SSAOstatus!=oldSSAOstatus)
        scene->setSSAO(SSAOstatus);
    float exposure = scene->getExposure();
    ImGui::DragFloat("exposure",&exposure,0.01,0.01,10.0);
    scene->setExposure(exposure);
    lightsParams();
    objectsParams();
    if(newLightWindowActive)
        newLightWindow();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Ui::lightsParams(){
    if(ImGui::TreeNode("Lights Parameters")){
        
        //display the light of light to choose from
        const char* items[100];
        std::vector<Light*>& lights = scene->getLights();
        std::vector<std::string> lightsNames;
        for (Light* li : lights)
            lightsNames.push_back(li->getName());

        for(int i = 0; i<lightsNames.size();i++){
            items[i] = lightsNames[i].c_str();
        }
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(64,64,255,255));
        if(ImGui::Button("Add a Light") && lights.size() < 100){
            newLightWindowActive = true;
            nlMult = 1.0;
        }
        ImGui::PopStyleColor();

        ImGui::ListBox("Select a Light to modify", &lightListIndex, items, lightsNames.size(), 5);
        Light* light = lights[lightListIndex];

        std::string lightText = "\n"+light->getName()+" parameters:\n";
        ImGui::Text(lightText.c_str());
        // Change parameters for selected light

        //diffuse color
        static float staticmult;
        if(lightListIndex != lastlightListIndex)
            staticmultSet = false;   
        if(!staticmultSet){
            staticmult = 1.0;
            staticmultSet = true;
        }
        ImGui::DragFloat("Color Multiplier",&staticmult,0.1,0.01,20.0);
        glm::vec3 diffuse = light->getDiffuse()/staticmult;
        ImGui::ColorEdit3("Color", &diffuse[0]);
        light->setDiffuse(diffuse*staticmult);              

        // change pos
        glm::vec3 pos = light->getPos();
        if(light->isDistant()){
            pos = glm::normalize(pos);
            ImGui::DragFloat3("Incoming position", &pos[0],0.01f,-1.0f,1.0f);
            
        } else {
            ImGui::DragFloat3("Position", &pos[0],0.1f,-10.0f,10.0f);
        }
        light->setPosition({pos[0],pos[1],pos[2]});
        
        //shadowmap params
        if(light->isDistant()){
            DistantLight* dl = dynamic_cast<DistantLight*>(light);
            if(light->hasShadowMap()){
                int res = dl->getSMRes();
                int res_tmp = res;
                ImGui::DragInt("ShadowMap Res", &res,32,128,4096);
                if(res_tmp != res){
                    dl->updateShadowMap(res,10.0,true);
                }
            }
        }
        
        // delete the light
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255,30,30,255));
        std::string deltext = "Delete "+ light->getName();
        if (ImGui::Button(deltext.c_str()) && lights.size() > 1) {
            if(lightListIndex < lights.size()-1){
                lights.erase(lights.begin()+lightListIndex);
            }
            else{
                lights.pop_back();
                lightListIndex-=1;
            }
        }
        ImGui::PopStyleColor();
        ImGui::TreePop();
    }
    lastlightListIndex = lightListIndex;
}


void Ui::objectsParams(){
    if(ImGui::TreeNode("Objects Parameters")){
        const char* items[200];
        std::vector<Model*>& models = scene->getModels();
        std::vector<std::string> modelsNames;
        for (auto* mo : models)
            modelsNames.push_back(mo->getName());

        for(int i = 0; i<modelsNames.size();i++){
            items[i] = modelsNames[i].c_str();
        } 

        ImGui::ListBox("Select a Model", &modelListIndex, items, modelsNames.size(), 5);
        Model* model = models[modelListIndex];

        std::string modelText = "\n"+model->getName()+" parameters:\n";
        ImGui::Text(modelText.c_str());
        // position
        glm::vec3 pos = model->getPosition();
        ImGui::DragFloat3("Position",&pos[0],0.01,-10.0,10.0);
        model->setPosition(pos);
        // position
        glm::vec3 scale = model->getScale();
        ImGui::DragFloat3("Scale",&scale[0],0.01,0.01,10.0);
        model->setScale(scale);

        // diffuse
        if(model->getShaderType() == PHONG){
            glm::vec3 diffuse = model->getDiffuse();
            ImGui::ColorEdit3("Diffuse",&diffuse[0]);
            model->setDiffuse(diffuse);
            glm::vec3 specular = model->getSpecular();
            ImGui::ColorEdit3("Specular",&specular[0]);
            model->setSpecular(specular);
            float shininess = model->getShininess();
            ImGui::DragFloat("Shininess",&shininess,1.0,5.0,256.0);
            model->setShininess(shininess);
        } 
        else if (model->getShaderType() == PBR){
            glm::vec3 albedo = model->getDiffuse();
            ImGui::ColorEdit3("Albedo",&albedo[0]);
            model->setDiffuse(albedo);
            float roughness = model->getRoughness();
            ImGui::DragFloat("Roughness",&roughness,0.01,0.00,1.0);
            model->setRoughness(roughness);
            float metallic = model->getMetallic();
            ImGui::DragFloat("Metallic",&metallic,0.01,0.00,1.0);
            model->setMetallic(metallic);
        }

        // tessellation
        ImGui::Text("Tessellation Parameters:");
        TESS_QUALITY tqual = model->getTesselationStatus();

        ImGui::Text("Presets:");
        std::string textBtn = std::string("Disabled");
        (tqual==DISABLED)?ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(32,32,128,255)):0;
        if(ImGui::Button(textBtn.c_str())){
            model->enableTesselation(DISABLED);
            model->loadShaders();
        }
        (tqual==DISABLED)?ImGui::PopStyleColor():0;

        textBtn = std::string("Low");
        (tqual==LOW)?ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(32,32,128,255)):0;
        if(ImGui::Button(textBtn.c_str())){
            model->enableTesselation(LOW);
            model->loadShaders();
        }
        (tqual==LOW)?ImGui::PopStyleColor():0;

        textBtn = std::string("Medium");
        (tqual==MEDIUM)?ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(32,32,128,255)):0;
        if(ImGui::Button(textBtn.c_str())){
            model->enableTesselation(MEDIUM);
            model->loadShaders();
        }
        (tqual==MEDIUM)?ImGui::PopStyleColor():0;


        textBtn = std::string("High");
        (tqual==HIGH)?ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(32,32,128,255)):0;
        if(ImGui::Button(textBtn.c_str())){
            model->enableTesselation(HIGH);
            model->loadShaders();
        }
        (tqual==HIGH)?ImGui::PopStyleColor():0;

        ImGui::Text("Texture Parameters:");
        if(model->hasTextures()){
            glm::vec2 texScale = model->getTexScaling();
            ImGui::DragFloat2("Texture Scaling",&texScale[0],0.1,0.1,20.0);
            model->setTexScaling(texScale);
        }
        

        ImGui::TreePop();
    }
}


void Ui::newLightWindow(){
    ImGui::Begin("Add a light");
    const char* items[] = { "Point Light", "Distant Light"};
    static int item_current = 0;
    ImGui::Combo("Light Type", &item_current, items, IM_ARRAYSIZE(items));

    switch(item_current){
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

void Ui::newPointLight(){
    static glm::vec3 nlPos;
    static glm::vec3 nlColor;

    ImGui::DragFloat3("position",&nlPos[0],0.01,-10.0,10.0);
    ImGui::DragFloat("Light Power Multiplier",&nlMult,0.1,0.1,100.0);
    ImGui::ColorEdit3("Color",&nlColor[0]);

    if(ImGui::Button("Add")){
        Light* pl = new PointLight(nlPos,nlColor*nlMult);
        nlMult = 1.0;
        scene->addLight(*pl);
        newLightWindowActive = false;
    }
}

void Ui::newDistantLight(){
    static glm::vec3 nlPos;
    static glm::vec3 nlColor;
    static bool shadowMap;
    static int shadowMapRes;

    ImGui::DragFloat3("Incoming Position",&nlPos[0],0.01,-1.0,1.0);
    ImGui::DragFloat("Light Power Multiplier",&nlMult,0.1,0.0,100.0);
    ImGui::ColorEdit3("Color",&nlColor[0]);
    ImGui::Checkbox("Enable Shadow Map", &shadowMap);
    if(shadowMap){
        ImGui::DragInt("resolution",&shadowMapRes,32,256,4096);
    }

    if(ImGui::Button("Add")){
        DistantLight* dl = new DistantLight{nlPos,nlColor*nlMult};
        if(shadowMap){
            dl->updateShadowMap(shadowMapRes,10.0f,true);
        }
            
        scene->addLight(*dl);
        newLightWindowActive = false;
    }
}