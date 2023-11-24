//
// Created by bat on 22/11/23.
//

#ifndef OBJECT_VIEWER_SCENEOPERATIONS_H
#define OBJECT_VIEWER_SCENEOPERATIONS_H

#include "globals.h"


namespace SceneOperations
{

    void initSceneLights()
    {
        Light l1 = Light();
        Light l2 = Light();

        l1.pos = glm::vec3(5,0,2);
        l2.pos = glm::vec3(-5,0,2);

        l1.color = glm::vec3((float)255/255,(float)190/255,(float)136/255);
        l2.color = glm::vec3((float)198/255,(float)216/255,(float)255/255);

        lights.push_back(l1);
        lights.push_back(l2);
    }

    int addLight()
    {
        if(lights.size() >= lightsMaxNumber){
            std::cout << "max lights number reached\n";
            return -1;
        }

        lights.push_back(Light());

        return 1;
    }

    int removeLight()
    {
        if(lights.size() == 0){
            std::cout << "there is no light\n";
            return -1;
        }

        lights.resize(lights.size()-1);

        return 1;
    }

    void openFile(std::string path)
    {
        std::cout << path << "\n";
    }
}

#endif //OBJECT_VIEWER_SCENEOPERATIONS_H
