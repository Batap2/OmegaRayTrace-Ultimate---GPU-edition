//
// Created by bat on 20/11/23.
//

#ifndef OBJECT_VIEWER_MATERIAL_H
#define OBJECT_VIEWER_MATERIAL_H

#include "deps/glm/glm.hpp"
enum MaterialType {
    Material_Diffuse_Blinn_Phong ,
    Material_Glass,
    Material_Mirror
};

enum LightingType {
    LightingType_Phong,
    LightingType_PBR
};

struct Texture{
    unsigned int height, width;
    std::vector<unsigned char> data;
};


struct Material {
    MaterialType mType;
    LightingType lType;

    glm::vec3 ambient_material;
    glm::vec3 diffuse_material;
    glm::vec3 specular_material;

    Texture diffuse_texture;

    double shininess;
    float index_medium;
    float transparency;

    bool emissive = false;

    // ---------- LightingType_PBR ----------
    glm::vec3 albedo;
    float metallic, roughness, ao;

    Material() {
        mType = Material_Diffuse_Blinn_Phong;
        lType = LightingType_Phong;
        transparency = 0.0;
        index_medium = 1.0;
        ambient_material = glm::vec3(0.0f, 0.0f, 0.0f);
        albedo = ambient_material;

    }
};
#endif //OBJECT_VIEWER_MATERIAL_H
