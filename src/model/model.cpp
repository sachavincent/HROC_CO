#include "model.hpp"
#include "scene.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <algorithm>


void Model::load(){

    // gen geometry buffers
    glGenBuffers(1, &m.vbo);
    glGenBuffers(1, &m.nbo);
    glGenBuffers(1, &m.tbo);
	glGenBuffers(1, &m.ebo);
    glGenVertexArrays(1, &m.vao);

	// Bind the vao
    glBindVertexArray(m.vao);

    // create and fill vertex data 
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(GLfloat), m.vertices.data(), GL_STATIC_DRAW);
	// set vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    glEnableVertexAttribArray(0);


    // copy normals to nbo
    glBindBuffer(GL_ARRAY_BUFFER, m.nbo);
    glBufferData(GL_ARRAY_BUFFER, m.normals.size() * sizeof(GLfloat), m.normals.data(), GL_STATIC_DRAW);
    // define array for normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    glEnableVertexAttribArray(1);

    // Copy texture array in element buffer
    glBindBuffer(GL_ARRAY_BUFFER, m.tbo);
    glBufferData(GL_ARRAY_BUFFER, m.textureCoord.size() * sizeof(GLfloat), m.textureCoord.data(), GL_STATIC_DRAW);
	// define array for texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
    glEnableVertexAttribArray(2);

	//copy indices to ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(GLfloat), m.indices.data(), GL_STATIC_DRAW);


    // Unbind the VAO
    glBindVertexArray(0);

	// load textures if defined
	if (m.diffuseMapPath != ""){
		m.diffuseMap = loadTexture(m.diffuseMapPath.c_str());
	}
	if (m.specularMapPath != ""){
		m.specularMap = loadTexture(m.specularMapPath.c_str());
	}
	if (m.heightMapPath != ""){
		m.heightMap = loadTexture(m.heightMapPath.c_str());
	}
	if (m.normalMapPath != ""){
		m.normalMap = loadTexture(m.normalMapPath.c_str());
	}
	if (m.AOMapPath != ""){
		m.AOMap = loadTexture(m.AOMapPath.c_str());
	}
	if(m.metallicMapPath != ""){
		m.metallicMap = loadTexture(m.metallicMapPath.c_str());
	}

	loadShaders();
}


void Model::render(Scene* _scene)  {
	Camera& cam = _scene->getCam();
	std::vector<Light*>& lights =  _scene->getLights();

    m.shader.use();
	//glm::vec3 axis{0,1,0};
	//m.rotation = glm::rotate(m.rotation,glm::radians(0.01f),axis);
    glm::mat4 model = m.translate*m.rotation*m.scale;

	m.shader.setMat4("model", model);
    m.shader.setMat4("view", cam.getView());
    m.shader.setMat4("projection", cam.getProj());
	m.shader.setVec2("screenSize", glm::vec2(cam.getResWidth(),cam.getResHeight()));
    m.shader.setVec3("viewPos", cam.getPos());
	m.shader.setFloat("exposure",_scene->getExposure());
    m.shader.setVec2("texScaling" , m.texScaling);
	
	if(m.shaderType == PHONG){
		m.shader.setFloat("material.specularStrength", 0.5f);
		m.shader.setFloat("material.shininess", m.shininess);
	}
	
	// bind diffuse/albedo texture
	glActiveTexture(GL_TEXTURE0);
	if (m.diffuseMap  != -1){
		m.shader.setInt("material.diffuseTex", 0);
		m.shader.setInt("material.albedoTex", 0);
		m.shader.setBool("material.hasTexture",true);
		glBindTexture(GL_TEXTURE_2D, m.diffuseMap);
	} else {
		m.shader.setVec3("material.diffuse", m.diffuseColor);
		m.shader.setVec3("material.albedo", m.diffuseColor);
		m.shader.setBool("material.hasTexture",false);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// bind specular/roughness texture
	glActiveTexture(GL_TEXTURE1);
	if(m.specularMap  != -1){
			m.shader.setInt("material.specularTex", 1);
			m.shader.setInt("material.roughnessTex", 1);
		glBindTexture(GL_TEXTURE_2D, m.specularMap);
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
		m.shader.setVec3("material.specular", m.specularColor);
		m.shader.setFloat("material.roughness", m.roughness);
	}
	//bind mellatlic texture (if PBR)
	glActiveTexture(GL_TEXTURE2);
	if(m.shaderType == PBR){
		if(m.metallicMap != -1){
			m.shader.setBool("material.hasMetallicTex",true);
			m.shader.setInt("material.metallicTex", 2);
			glBindTexture(GL_TEXTURE_2D, m.metallicMap);
		} else {
			m.shader.setFloat("material.metallic",m.metallic);
			m.shader.setBool("material.hasMetallicTex",false);
		}
	} else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	

	// bind displacement map texture
	glActiveTexture(GL_TEXTURE3);
    if(m.heightMap  != -1){
        m.shader.setInt("dispMap", 3);
		m.shader.setFloat("dispStrength", m.displacementStrength);
        glBindTexture(GL_TEXTURE_2D, m.heightMap);
    } else {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE5);
	if(m.normalMap != -1){
		glBindTexture(GL_TEXTURE_2D, m.normalMap);
		m.shader.setInt("material.normalMap", 5);
		m.shader.setBool("material.hasNormalMap",true);
	} else {
		m.shader.setBool("material.hasNormalMap",false);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glActiveTexture(GL_TEXTURE6);
	if(m.AOMap != -1){
		glBindTexture(GL_TEXTURE_2D, m.AOMap);
		m.shader.setInt("material.AOmap", 6);
		m.shader.setBool("material.hasAOMap",true);
	} else {
		m.shader.setBool("material.hasAOMap",false);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	
	int j = 0;
    for(uint32_t i = 0; i<std::min(lights.size(),(size_t)MAXLIGHTS); i++){
		if(lights[i]->hasShadowMap()){
			glActiveTexture(GL_TEXTURE7+j);
			DistantLight* li = dynamic_cast<DistantLight*>(lights[i]);
			glBindTexture(GL_TEXTURE_2D, li->getDepthTexture());

			m.shader.setMat4("lightSpaceMatrix["+std::to_string(j)+"]", li->getLightSpacematrix());
			m.shader.setInt("shadowMap["+std::to_string(j)+"]", 7+j);
			m.shader.setInt("lights["+   std::to_string(i) + "].shadowMapId", j);
			j++;
		} else {
			m.shader.setInt("lights["+   std::to_string(i) + "].shadowMapId", -1);
		}
		if(lights[i]->isDistant()){
			m.shader.setBool("lights["+   std::to_string(i) + "].distant",true);
		} else {
			m.shader.setBool("lights["+   std::to_string(i) + "].distant",false);
		}

		m.shader.setBool("lights["+   std::to_string(i) + "].enabled",1);

        m.shader.setVec3("lights["+   std::to_string(i) + "].position",  lights[i]->getPos());
		if(m.shaderType == PHONG){
			m.shader.setVec3("lights[" +  std::to_string(i) + "].ambient",   lights[i]->getAmbiant());
	    	m.shader.setVec3("lights[" +  std::to_string(i) + "].diffuse",   lights[i]->getDiffuse());
	    	m.shader.setVec3("lights[" +  std::to_string(i) + "].specular",  lights[i]->getSpecular());

	    	m.shader.setFloat("lights[" + std::to_string(i) + "].constant",  lights[i]->getConstant());
	    	m.shader.setFloat("lights[" + std::to_string(i) + "].linear",    lights[i]->getLinear());
	    	m.shader.setFloat("lights[" + std::to_string(i) + "].quadratic", lights[i]->getQuadratic());
		} 
		else if (m.shaderType == PBR){
			m.shader.setVec3("lights[" +  std::to_string(i) + "].color",   lights[i]->getDiffuse());
		}
        
    }
	if (lights.size()<MAXLIGHTS){
		for (size_t i = lights.size(); i<MAXLIGHTS; i++){
			m.shader.setBool("lights["+   std::to_string(i) + "].enabled",0);
		}
	}
	
	if(m.tqual != DISABLED){
		if(m.tqual == LOW){
				m.shader.setInt("tes_lod0", 16); //under 2 unit distance
				m.shader.setInt("tes_lod1", 4); //2 to 5 distance
				m.shader.setInt("tes_lod2", 2);  // farther than 5
			} else if (m.tqual == MEDIUM){
				m.shader.setInt("tes_lod0", 32); //under 2 unit distance
				m.shader.setInt("tes_lod1", 8); //2 to 5 distance
				m.shader.setInt("tes_lod2", 2);  // farther than 5
			} else if(m.tqual == HIGH){
				m.shader.setInt("tes_lod0", 64); //under 2 unit distance
				m.shader.setInt("tes_lod1", 16); //2 to 5 distance
				m.shader.setInt("tes_lod2", 4);  // farther than 5
			}
	} 

    glBindVertexArray(m.vao);
    
	if(m.tqual != DISABLED){
		glDrawElements(GL_PATCHES , m.indices.size(), GL_UNSIGNED_INT, nullptr);
	} else {
		glDrawElements(GL_TRIANGLES , m.indices.size(), GL_UNSIGNED_INT, nullptr);
	}
	glBindVertexArray(0);

}

void Model::renderForDepth(Shader& _shader){

	_shader.use();
	glm::mat4 model = m.translate*m.rotation*m.scale;

	_shader.setMat4("model", model);

	glBindVertexArray(m.vao);
	
	glDrawElements(GL_TRIANGLES , m.indices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

void Model::loadShaders(){
	std::string frag;
	if(m.shaderType == PHONG){
		frag = "shaders/phong.frag";
	} else if (m.shaderType == PBR){
		frag = "shaders/pbr.frag";
	}
	// load right shader
	if(m.tqual != DISABLED){
		if (m.heightMapPath != ""){
			m.shader = {"shaders/tessellation/tess.vert", frag,
				 "shaders/tessellation/tessBump.tesc",
				 "shaders/tessellation/tessBump.tese"};
		} else {
			m.shader = {"shaders/tessellation/tess.vert", frag,
				 "shaders/tessellation/tessPN.tesc",
				 "shaders/tessellation/tessPN.tese"};
		}
		
	} else {
		m.shader = {"shaders/default.vert", frag};
	}
}



Model& Model::setScale(glm::vec3 _scale){
    m.scale = glm::mat4(1.0);
    m.scale = glm::scale(m.scale,_scale);
    return *this;
}

Model& Model::setRotation(float _angle, glm::vec3 _axis){
    m.rotation = glm::mat4(1.0);
    m.rotation = glm::rotate(m.rotation, glm::radians(_angle),_axis);
    return *this;
}

Model& Model::setPosition(glm::vec3 _pos){
    m.translate = glm::mat4{1.0};
    m.translate = glm::translate(m.translate, _pos);
    return *this;
}




Model& Model::setTexDiffuse(std::string _path){
    m.diffuseMapPath = _path;
    return *this;
}

Model& Model::setTexSpecular(std::string _path){
    m.specularMapPath = _path;
    return *this;
}

Model& Model::setShininess(float _shininess){
	m.shininess = _shininess;
	return *this;
}

Model& Model::setTexAlbedo(std::string _path){
    m.diffuseMapPath = _path;
    return *this;
}

Model& Model::setTexRoughness(std::string _path){
    m.specularMapPath = _path;
    return *this;
}

Model& Model::setTexMetallic(std::string _path){
    m.metallicMapPath = _path;
    return *this;
}

Model& Model::setTexHeight( std::string _heightPath){
    m.heightMapPath = _heightPath;
    m.tqual = MEDIUM;
    return *this;
}

Model& Model::setTexNormal( std::string _normalPath){
    m.normalMapPath = _normalPath;
    return *this;
}

Model& Model::setTexAO( std::string _AOPath){
    m.AOMapPath = _AOPath;
    return *this;
}

Model& Model::enableTesselation(TESS_QUALITY _quality){
    m.tqual = _quality;
    return *this;
}

Model& Model::enableTesselation(){
    m.tqual = MEDIUM;
    return *this;
}
Model& Model::disableTesselation(){
    m.tqual = DISABLED;
    return *this;
}

Model& Model::setDiffuse(glm::vec3 _color){
	m.diffuseColor = _color;
	return *this;
}

Model& Model::setAlbedo(glm::vec3 _color){
	m.diffuseColor = _color;
	return *this;
}

Model& Model::setSpecular(glm::vec3 _color){
	m.specularColor = _color;
	return *this;
}

Model& Model::setRoughness(float _roughness){
	m.roughness = _roughness;
	return *this;
}

Model& Model::setMetallic(float _metallic){
	m.metallic = _metallic;
	return *this;
}

Model& Model::displacementStrength(float _strength){
    m.displacementStrength = _strength;
    return *this;
}

Model& Model::setTexScaling( glm::vec2 _scale){
    m.texScaling = _scale;
    return *this;
}

Model& Model::setShaderType(SHADER_TYPE _type){
	m.shaderType = _type;
	if(m.shaderLoaded){
		loadShaders();
	}
	return *this;
}
