#include "object.hpp"
#include "scene.hpp"
#include "utils/utils.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

 unsigned int FileObject::instance_counter = 0;

FileObject::FileObject(const std::string &_path,std::string _modelkey)
    : Object(_path.substr(_path.find_last_of("/") + 1) + "_" + std::to_string(instance_counter),
    _modelkey)
{
    if(_modelkey.size() == 0)
    {
        std::cout << "test" << std::endl;
    }

    instance = instance_counter++;
}