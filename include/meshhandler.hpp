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


 using namespace std;

struct OBJECT_DATA;
class Object;


//Contains all data for each objects
class MeshHandler
{
private:
    vector<OBJECT_DATA> datas;
    map<string,OBJECT_DATA*> mapData;
public:
    bool addData(string key,OBJECT_DATA value)
    {
        if(mapData.count(key) == 1 || key.size() == 0)
        {
            return false;
        }
        datas.push_back(OBJECT_DATA(value));
        mapData[key] = &(datas[datas.size() - 1]);
        return true;
    }

    bool contain(string key) {return mapData.count(key);}

    OBJECT_DATA* getData(string key) 
    {
        if(mapData.count(key) == 1)
        {
            return mapData[key];
        }
        return nullptr;
    }

    size_t getSizeData()
    {
        return datas.size();
    }

    inline OBJECT_DATA* getData(int i){return &datas[i];}


    DrawElementsCommand* getCmds(std::vector<shared_ptr<Object>>& _objects);
    void getBuffers(vector<Vertex>& vert,vector<GLuint>& ind)
    {
        for (int i = 0; i < datas.size(); i++)
        {
            OBJECT_DATA* objData = getData(i);
            std::vector<Vertex> vec(objData->vertices, objData->vertices + objData->numVertices);
            vert.insert(vert.end(), vec.begin(), vec.end());

            std::vector<GLuint> tempind(objData->indices, objData->indices + objData->numIndices);
            ind.insert(ind.end(), tempind.begin(), tempind.end());
        }
    }

   static MeshHandler* getSingleton() {
        static MeshHandler* singleton = nullptr;
        if(singleton == nullptr)
        {
            singleton = new MeshHandler();
        }
        return singleton;
   }
};

//Interface on each object to attribute a model by key 
class MeshFilter
{
private:

    MeshFilter() :handlerSingleton(MeshHandler::getSingleton()),key(){}
protected:
    MeshHandler* handlerSingleton;
    string key;
public:
    MeshFilter(string _k) :handlerSingleton(MeshHandler::getSingleton()),key(_k){}


    OBJECT_DATA* getMeshData() {if(key.size() == 0){return nullptr;} return handlerSingleton->getData(key);};
    string getMeshKey() const {return key;}
    OBJECT_BOUNDS getObjectBounds() {return getMeshData()->bounds ;}
    bool setMesh(string _key)
    {
        if(handlerSingleton->contain(_key))
            return false;

        key = _key;
        return true;
    }

    

};
class MeshRenderer
{
private:
    bool isVisible;
    MeshFilter* meshFilter;
public:
    MeshRenderer(): isVisible(true){meshFilter = nullptr;}
    MeshRenderer(MeshFilter* _filt):meshFilter(_filt), isVisible(true){}
    void setVisibility(bool vis){isVisible = vis;}
    void toggleVisibility(){isVisible = !isVisible;}
    bool is_Visible(){return isVisible;}
};

class MeshLoader
{
private:
    MeshHandler* handlerSingleton;
    MeshLoader():handlerSingleton(MeshHandler::getSingleton()) {}

public:
    void generateBaseModels();
    void MeshLoader::loadModel(const std::string &_path,const string& key);
    OBJECT_DATA processMesh(aiMesh *_mesh, const aiScene *_scene);

   static MeshLoader* getSingleton() {
        static MeshLoader* singleton = nullptr;
        if(singleton == nullptr)
        {
            singleton = new MeshLoader();
        }
        return singleton;
   }
};


#endif