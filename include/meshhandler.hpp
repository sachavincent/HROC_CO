#ifndef MESHHANDLER_H
#define MESHHANDLER_H

#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <list>

#include "utils/utils.hpp"
//#include "object.hpp"
#include <string>
#include <time.h>
#include <chrono>
#include <map>
#include <ctime>

#include <string>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct OBJECT_DATA;
class Object;

// Singleton
// Contains all data for each Models by key
// and distribute the data when needed
// Can build gl draw commands array from object list
class MeshHandler
{
private:
    std::vector<OBJECT_DATA *> datas;
    std::map<std::string, OBJECT_DATA *> mapData;

public:
    bool addData(std::string key, OBJECT_DATA value)
    {
        if (mapData.count(key) == 1 || key.size() == 0)
        {
            return false;
        }
        OBJECT_DATA *d = new OBJECT_DATA(value);
        datas.push_back(d);
        mapData[key] = d;
        return true;
    }

    bool contain(std::string key) { return mapData.count(key); }

    OBJECT_DATA *getData(std::string key)
    {
        if (mapData.count(key) == 1)
        {
            return mapData[key];
        }
        return nullptr;
    }

    size_t getSizeData()
    {
        return datas.size();
    }

    inline OBJECT_DATA *getData(int i) { return datas[i]; }

    DrawElementsCommand *getCmdsForSubset(const std::vector<std::shared_ptr<Object>> &_objects, int *cmdCount);
    DrawElementsCommand *getCmds(std::vector<std::shared_ptr<Object>> &_objects, int *cmdCount);
    /*void getBuffers(vector<Vertex>& vert,vector<GLuint>& ind)
    {
        for (int i = 0; i < datas.size(); i++)
        {
            OBJECT_DATA* objData = getData(i);
            std::vector<Vertex> vec(objData->vertices, objData->vertices + objData->numVertices);
            vert.insert(vert.end(), vec.begin(), vec.end());

            std::vector<GLuint> tempind(objData->indices, objData->indices + objData->numIndices);
            ind.insert(ind.end(), tempind.begin(), tempind.end());
        }
    }*/
    void getBuffers(std::vector<std::shared_ptr<Object>> &_objects, std::vector<Vertex> &vert, std::vector<GLuint> &ind);

    static MeshHandler *getSingleton()
    {
        static MeshHandler *singleton = nullptr;
        if (singleton == nullptr)
        {
            singleton = new MeshHandler();
        }
        return singleton;
    }
};

// Interface on each object to attribute a model by key
// Can ask the Handler to get ObjectData
class MeshFilter
{
private:
    MeshFilter() : handlerSingleton(MeshHandler::getSingleton()), key() {}

protected:
    MeshHandler *handlerSingleton;
    std::string key;

public:
    MeshFilter(std::string _k) : handlerSingleton(MeshHandler::getSingleton()), key(_k) {}

    OBJECT_DATA *getMeshData()
    {
        if (key.size() == 0)
        {
            return nullptr;
        }
        return handlerSingleton->getData(key);
    };
    std::string getMeshKey() const { return key; }
    OBJECT_BOUNDS getObjectBounds() { return getMeshData()->bounds; }
    bool setMesh(std::string _key)
    {
        if (handlerSingleton->contain(_key))
            return false;

        key = _key;
        return true;
    }
};

// Component on each object to control draw behavior
class MeshRenderer
{
private:
    bool isVisible;
    MeshFilter *meshFilter;

public:
    MeshRenderer() : isVisible(true) { meshFilter = nullptr; }
    MeshRenderer(MeshFilter *_filt) : meshFilter(_filt), isVisible(true) {}
    void setVisibility(bool vis) { isVisible = vis; }
    void toggleVisibility() { isVisible = !isVisible; }
    bool is_Visible() { return isVisible; }
};

// On project startup load base models(Sphere,Cube,..) and store them in the Handler
// Can also load a model and store it in the MeshHandler
class MeshLoader
{
private:
    MeshHandler *handlerSingleton;
    MeshLoader() : handlerSingleton(MeshHandler::getSingleton()) {}

public:
    void generateBaseModels();
    void loadModel(const std::string &_path, const std::string &key);
    OBJECT_DATA processMesh(aiMesh *_mesh, const aiScene *_scene);

    static MeshLoader *getSingleton()
    {
        static MeshLoader *singleton = nullptr;
        if (singleton == nullptr)
        {
            singleton = new MeshLoader();
        }
        return singleton;
    }
};

#endif