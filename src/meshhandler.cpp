#include "meshhandler.hpp"
#include "object.hpp"
#include "scene.hpp"
#include "utils/utils.hpp"

void MeshLoader::generateBaseModels()
{
    OBJECT_DATA cubeData;

    cubeData.bounds.min = {-0.5f, -0.5f, -0.5f};
    cubeData.bounds.max = {0.5f, 0.5f, 0.5f};

    std::vector<GLfloat> vertices = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f};

    std::vector<GLfloat> normals = {
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,

        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,

        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f};

    std::vector<GLfloat> textureCoord = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f};

    std::vector<GLuint> indices = {
        0, 2, 1,
        3, 5, 4,

        6, 7, 8,
        9, 10, 11,

        12, 13, 14,
        15, 16, 17,

        18, 20, 19,
        21, 23, 22,

        24, 25, 26,
        27, 28, 29,

        30, 32, 31,
        33, 35, 34};

    cubeData.numVertices = vertices.size() / 3;
    cubeData.vertices = new Vertex[cubeData.numVertices];
    for (size_t i = 0; i < cubeData.numVertices; i++)
    {
        Vertex vertex;
        vertex.Position = {vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]};
        vertex.Normal = {normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]};
        vertex.TexCoord = {textureCoord[i * 2], textureCoord[i * 2 + 1]};
        cubeData.vertices[i] = vertex;
    }

    cubeData.numIndices = indices.size();
    cubeData.indices = new GLuint[cubeData.numIndices];
    for (size_t i = 0; i < cubeData.numIndices; i++)
        cubeData.indices[i] = indices[i];

    handlerSingleton->addData("cube", cubeData);

    OBJECT_DATA planeData;
    planeData.bounds.min = {-0.5f, -0.5f, 0.0f};
    planeData.bounds.max = {0.5f, 0.5f, 0.0f};

    vertices = {
        -0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f};

    normals = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f};

    textureCoord = {
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f};

    indices = {
        1, 2, 0,
        1, 3, 2};

    planeData.numVertices = vertices.size() / 3;
    planeData.vertices = new Vertex[planeData.numVertices];
    for (size_t i = 0; i < planeData.numVertices; i++)
    {
        Vertex vertex;
        vertex.Position = {vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]};
        vertex.Normal = {normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]};
        vertex.TexCoord = {textureCoord[i * 2], textureCoord[i * 2 + 1]};
        planeData.vertices[i] = vertex;
    }

    planeData.numIndices = indices.size();
    planeData.indices = new GLuint[planeData.numIndices];
    for (size_t i = 0; i < planeData.numIndices; i++)
        planeData.indices[i] = indices[i];

    handlerSingleton->addData("plane", planeData);

    OBJECT_DATA sphereData;

    int _nCols = 25;
    int _nRows = 20;
    float _radius = 1.0f;
    float x, y, z, xy, s, t;

    sphereData.bounds.min = {-_radius, -_radius, -_radius};
    sphereData.bounds.max = {_radius, _radius, _radius};

    vertices.clear();
    normals.clear();
    indices.clear();
    textureCoord.clear();

    vertices.reserve(3 * _nCols * _nRows);
    normals.reserve(3 * _nCols * _nRows);
    indices.reserve(6 * _nCols * _nRows);
    textureCoord.reserve(2 * _nCols * _nRows);

    float pi = 3.1415f;
    float r_inv = 1.0f / _radius; // for normals
    float row_step = 2 * pi / _nRows;
    float col_step = pi / _nCols;

    for (int i = 0; i <= _nCols; ++i)
    {
        xy = _radius * cosf(pi / 2 - i * col_step);
        z = _radius * sinf(pi / 2 - i * col_step);
        int a = i * (_nRows + 1);
        int b = a + _nRows + 1;

        for (int j = 0; j <= _nRows; ++j, ++a, ++b)
        {
            // add vertices
            x = xy * cosf(j * row_step);
            y = xy * sinf(j * row_step);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // add normals
            normals.push_back(x * r_inv);
            normals.push_back(y * r_inv);
            normals.push_back(z * r_inv);

            // add indices
            if (i != 0 && i <= (_nCols - 1))
            {
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(a + 1);
            }
            if (i <= (_nCols - 2))
            {
                indices.push_back(a + 1);
                indices.push_back(b);
                indices.push_back(b + 1);
            }

            s = (float)j / _nRows;
            t = (float)i / _nCols;
            textureCoord.push_back(s);
            textureCoord.push_back(t);
        }
    }

    sphereData.numVertices = vertices.size() / 3;
    sphereData.vertices = new Vertex[sphereData.numVertices];
    for (size_t i = 0; i < sphereData.numVertices; i++)
    {
        Vertex vertex;
        vertex.Position = {vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]};
        vertex.Normal = {normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]};
        vertex.TexCoord = {textureCoord[i * 2], textureCoord[i * 2 + 1]};
        sphereData.vertices[i] = vertex;
    }

    sphereData.numIndices = indices.size();
    sphereData.indices = new GLuint[sphereData.numIndices];
    for (size_t i = 0; i < sphereData.numIndices; i++)
        sphereData.indices[i] = indices[i];

    handlerSingleton->addData("sphere", sphereData);
}

void MeshLoader::loadModel(const std::string &_path, const std::string &keyModel)
{
    bool _smoothNormals = true;

    std::string abs_path = Utils::workingDirectory() + _path;

    // skip loading if same object already exists
    if (!handlerSingleton->contain(keyModel))
    {
        Assimp::Importer importer;

        std::cout << "loading object from file : " << abs_path << " ..." << std::endl;

        const aiScene *scene =
            _smoothNormals ? importer.ReadFile(abs_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals)
                           : importer.ReadFile(abs_path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        if (scene->mNumMeshes != 1)
        {
            std::cerr << "File contains too many meshes" << std::endl;
            return;
        }

        aiMesh *mesh = scene->mMeshes[0];
        OBJECT_DATA data = processMesh(mesh, scene);
        handlerSingleton->addData(keyModel, data);
    }
}

OBJECT_DATA MeshLoader::processMesh(aiMesh *_mesh, const aiScene *_scene)
{
    OBJECT_DATA data;

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> textureCoord;
    std::vector<GLuint> indices;

    vertices.reserve(_mesh->mNumVertices * 3);
    normals.reserve(_mesh->mNumVertices * 3);
    indices.reserve(_mesh->mNumFaces * 3);
    // add vertices
    for (size_t i = 0; i < _mesh->mNumVertices; i++)
    {
        vertices.insert(vertices.end(), {_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z});
    }
    // add normals
    for (size_t i = 0; i < _mesh->mNumVertices; i++)
    {
        normals.insert(normals.end(), {_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z});
    }
    // add indices
    for (size_t i = 0; i < _mesh->mNumFaces; i++)
    {
        aiFace face = _mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // add texture coordinates
    if (_mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
        for (size_t i = 0; i < _mesh->mNumVertices; i++)
            textureCoord.insert(textureCoord.end(), {_mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y});
    }

    // center object so that centroid is at (0,0,0)
    // this important for scaling/rotation etc..
    double meanX = 0, meanY = 0, meanZ = 0;

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        meanX += vertices[i];
        meanY += vertices[i + 1];
        meanZ += vertices[i + 2];
    }

    size_t ptsNum = vertices.size() / 3;
    meanX /= ptsNum;
    meanY /= ptsNum;
    meanZ /= ptsNum;

    data.bounds.max = glm::vec3{-FLT_MAX};
    data.bounds.min = glm::vec3{FLT_MAX};

    for (size_t i = 0; i < vertices.size(); i += 3)
    {
        vertices[i] -= (GLfloat)meanX;
        vertices[i + 1] -= (GLfloat)meanY;
        vertices[i + 2] -= (GLfloat)meanZ;

        data.bounds.max.x = std::max(data.bounds.max.x, vertices[i]);
        data.bounds.max.y = std::max(data.bounds.max.y, vertices[i + 1]);
        data.bounds.max.z = std::max(data.bounds.max.z, vertices[i + 2]);

        data.bounds.min.x = std::min(data.bounds.min.x, vertices[i]);
        data.bounds.min.y = std::min(data.bounds.min.y, vertices[i + 1]);
        data.bounds.min.z = std::min(data.bounds.min.z, vertices[i + 2]);
    }

    data.numVertices = vertices.size() / 3;
    data.vertices = new Vertex[data.numVertices];
    for (size_t i = 0; i < data.numVertices; i++)
    {
        Vertex vertex;
        vertex.Position = {vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]};
        vertex.Normal = {normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]};
        if (_mesh->mTextureCoords[0])
            vertex.TexCoord = {textureCoord[i * 2], textureCoord[i * 2 + 1]};
        else
            vertex.TexCoord = {0, 0};

        data.vertices[i] = vertex;
    }

    data.numIndices = indices.size();
    data.indices = new GLuint[data.numIndices];
    for (size_t i = 0; i < data.numIndices; i++)
        data.indices[i] = indices[i];

    return data;
}

void MeshHandler::getBuffers(std::vector<std::shared_ptr<Object>> &_objects, std::vector<Vertex> &vert, std::vector<GLuint> &ind)
{
    std::vector<std::string> objOrder;
    std::map<std::string, OBJECT_DATA *> temp;
    for (auto it : _objects)
    {
        if (it->getObjectData() != nullptr)
        {
            OBJECT_DATA *data = it->getObjectData();
            std::string keyModel = it->getModelKey();
            if (temp.count(keyModel) == 0)
            {
                objOrder.push_back(keyModel);
                temp[keyModel] = data;
            }
        }
    }

    for (auto obj : objOrder)
    {
        OBJECT_DATA *data = temp[obj];
        std::vector<Vertex> vec(data->vertices, data->vertices + data->numVertices);
        vert.insert(vert.end(), vec.begin(), vec.end());

        std::vector<GLuint> tempind(data->indices, data->indices + data->numIndices);
        ind.insert(ind.end(), tempind.begin(), tempind.end());
    }
}

DrawElementsCommand *MeshHandler::getCmds(std::vector<std::shared_ptr<Object>> &_objects, int *cmdCount)
{
    GLuint baseVert = 0;
    GLuint baseIdx = 0;
    GLuint baseInstance = 0;

    std::vector<std::string> objOrder;
    std::map<std::string, OBJECT_DATA *> temp;
    std::map<std::string, std::vector<std::shared_ptr<Object>>> countMap;
    for (auto obj : _objects)
    {
        if (obj->getObjectData() != nullptr)
        {
            std::string keyModel = obj->getModelKey();
            if (temp.count(keyModel) == 0)
            {
                objOrder.push_back(keyModel);
                temp[keyModel] = obj->getObjectData();
                countMap[keyModel].push_back(obj);
            }
            else
            {
                countMap[keyModel].push_back(obj);
            }
        }
    }

    DrawElementsCommand *cmds = new DrawElementsCommand[temp.size()];

    size_t cmdId = 0;
    // for (auto it : temp)
    for (size_t cmdId = 0; cmdId < objOrder.size(); cmdId++)
    {
        std::string keyModel = objOrder[cmdId];
        OBJECT_DATA *data = temp[keyModel];
        std::vector<std::shared_ptr<Object>> objList = countMap[keyModel];
        int nbObj = int(objList.size());

        cmds[cmdId].vertexCount = data->numIndices;
        cmds[cmdId].instanceCount = nbObj;
        cmds[cmdId].firstIndex = baseIdx;
        cmds[cmdId].baseVertex = baseVert;
        cmds[cmdId].baseInstance = baseInstance;

        baseIdx += data->numIndices;
        baseVert += data->numVertices;
        baseInstance += nbObj;

        for (int i = 0; i < nbObj; i++)
        {
            DrawElementsCommand cmd = cmds[cmdId];
            cmd.instanceCount = 1;
            cmd.baseInstance += i;
            objList[i]->setCommand(cmd);
        }
    }
    *cmdCount = temp.size();
    return cmds;
}

DrawElementsCommand *MeshHandler::getCmdsForSubset(const std::vector<std::shared_ptr<Object>> &_objects, int *cmdCount)
{
    std::vector<DrawElementsCommand> commands;
    size_t i = 0;
    do
    {
        std::shared_ptr<Object> obj = _objects[i];
        std::string currentKeyModel = obj->getModelKey();
        GLuint currentBaseInstance = obj->getCommand().baseInstance;
        std::string keyModel = obj->getModelKey();
        int nbConsecutiveObj = 0;

        while (currentKeyModel == keyModel && i < _objects.size())
        {
            nbConsecutiveObj++;
            if (i == _objects.size() - 1)
                break;
            std::shared_ptr<Object> nextObj = _objects[++i];
            keyModel = nextObj->getModelKey();
            GLuint baseInstance = nextObj->getCommand().baseInstance;
            if (keyModel == currentKeyModel && baseInstance != currentBaseInstance + 1)
                break;
            currentBaseInstance++;
        }
        DrawElementsCommand command = obj->getCommand();
        command.instanceCount = nbConsecutiveObj;
        commands.push_back(command);
    } while (i < _objects.size() - 1);

    DrawElementsCommand *cmds = new DrawElementsCommand[commands.size()];
    for (size_t i = 0; i < commands.size(); i++)
        cmds[i] = commands[i];

    *cmdCount = commands.size();
    return cmds;
}