//
// Created by bat on 20/11/23.
//

#include "deps/glm/glm.hpp"

#ifndef OBJECT_VIEWER_CAMERA_H
#define OBJECT_VIEWER_CAMERA_H
#include <CL/cl2.hpp>
class Camera{

public:

    Camera(){};
    ~Camera(){};


    glm::vec3 cameraPos = glm::vec3(0.0f,2.0f,5.0f);

    glm::vec3 cameraDirection = glm::vec3(0.0f,-0.25f,-0.75f);;

    glm::vec3 cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    // Fonction pour obtenir les données de la caméra sous forme de tableau
    cl_float* getCameraData() {
        cl_float* data = new cl_float[9];
        data[0] = cameraPos.x;
        data[1] = cameraPos.y;
        data[2] = cameraPos.z;
        data[3] = cameraDirection.x;
        data[4] = cameraDirection.y;
        data[5] = cameraDirection.z;
        data[6] = cameraRight.x;
        data[7] = cameraRight.y;
        data[8] = cameraRight.z;
        return data;
    }

    // Fonction pour afficher la position de la caméra
    void printCameraPosition() const {
        std::cerr << "Camera Position: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")\n";
        std::cerr << "Camera Direction: (" << cameraDirection.x << ", " << cameraDirection.y << ", " << cameraDirection.z << ")\n";
        std::cerr << "Camera Right (" << cameraRight.x << ", " << cameraRight.y << ", " << cameraRight.z << ")\n";


    }

};

#endif //OBJECT_VIEWER_CAMERA_H
