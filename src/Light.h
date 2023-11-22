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


    Light(){
        intensity = 1;
        powerCorrection = 1;
        type = LightType_Spherical;
        pos = glm::vec3(0,0,0);
        color = glm::vec3(1,1,1);
    }
    ~Light(){};

    glm::vec3 color;
    bool isInCamSpace;

    LightType type;
    glm::vec3 pos;

    float radius;
    float intensity;

    //Square quad;

    float powerCorrection;
};
#endif //OBJECT_VIEWER_LIGHT_H
