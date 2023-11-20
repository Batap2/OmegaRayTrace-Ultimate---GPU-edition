//
// Created by bat on 20/11/23.
//

#include "deps/glm/glm.hpp"
#include "globals.h"

#ifndef OBJECT_VIEWER_CAMERA_H
#define OBJECT_VIEWER_CAMERA_H

class Camera{

public:

    Camera(){std::cout<<"yesy";};
    ~Camera(){};


    glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,0.0f);

    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

    glm::vec3 cameraRight = glm::normalize(glm::cross(upinit, cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);


};

#endif //OBJECT_VIEWER_CAMERA_H
