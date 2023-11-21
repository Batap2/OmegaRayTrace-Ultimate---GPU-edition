//
// Created by bat on 20/11/23.
//

#include "deps/glm/glm.hpp"

#ifndef OBJECT_VIEWER_CAMERA_H
#define OBJECT_VIEWER_CAMERA_H

class Camera{

public:

    Camera(){std::cout<<"camera";};
    ~Camera(){};


    glm::vec3 cameraPos = glm::vec3(0.0f,2.0f,5.0f);

    glm::vec3 cameraDirection = glm::vec3(0.0f,-0.25f,-0.75f);;

    glm::vec3 cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);


};

#endif //OBJECT_VIEWER_CAMERA_H
