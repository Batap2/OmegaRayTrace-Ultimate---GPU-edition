//
// Created by bat on 22/11/23.
//

#ifndef OBJECT_VIEWER_SCENEOPERATIONS_H
#define OBJECT_VIEWER_SCENEOPERATIONS_H

#include "globals.h"
#include "MeshLoader.h"

namespace SceneOperations
{

    void initSceneLights()
    {
        auto* l1 = new Light();
        auto* l2 = new Light();

        l1->pos = glm::vec3(5,0,2);
        l2->pos = glm::vec3(-5,0,2);

        l1->color = glm::vec3((float)255/255,(float)190/255,(float)136/255);
        l2->color = glm::vec3((float)198/255,(float)216/255,(float)255/255);

        scene_lights.push_back(l1);
        scene_lights.push_back(l2);
    }

    void destroyScene(){
        for (auto& lightPtr : scene_lights) {
            delete lightPtr;
        }

        for (auto& meshPtr : scene_meshes) {
            delete meshPtr;
        }
    }

    int addLight()
    {
        if(scene_lights.size() >= lightsMaxNumber){
            std::cout << "max scene_lights number reached\n";
            return -1;
        }

        Light l;

        scene_lights.push_back(&l);

        return 1;
    }

    int removeLight()
    {
        if(scene_lights.size() == 0){
            std::cout << "there is no light\n";
            return -1;
        }

        scene_lights.resize(scene_lights.size() - 1);

        return 1;
    }

    void openFile(std::string path)
    {
        std::cout << path << "\n";
        MeshLoader::import(path);
    }
}

#endif //OBJECT_VIEWER_SCENEOPERATIONS_H
