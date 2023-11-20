//
// Created by bat on 20/11/23.
//

#include "deps/glm/glm.hpp"

#ifndef OBJECT_VIEWER_LIGHT_H
#define OBJECT_VIEWER_LIGHT_H
class Light{
public:
    enum LightType {
        LightType_Spherical,
        LightType_Quad
    };


    Light() : powerCorrection(1.0) {}
    ~Light(){};

    glm::vec3 material;
    bool isInCamSpace;

    LightType type;
    glm::vec3 pos;

    float radius;

    //Square quad;

    float powerCorrection;
};
#endif //OBJECT_VIEWER_LIGHT_H
