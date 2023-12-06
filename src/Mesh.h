#ifndef MESH_H
#define MESH_H

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

#include "deps/glm/glm.hpp"
#include "Material.h"

struct UVCoord{
    float u, v;
    UVCoord(float _u, float _v) : u(_u), v(_v) {}
    UVCoord() : u(0.0f), v(0.0f) {}
};

struct Tri{
    uint32_t vertices[3];
    Tri(uint32_t a, uint32_t b, uint32_t c){
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
    }
    Tri(){
        vertices[0] = 0;
        vertices[1] = 0;
        vertices[2] = 0;
    }
};

class Mesh{
private:
    GLuint vertex_array, vertex_buffer, normal_buffer, index_buffer;
public:
    GLuint VAO, VBO, EBO, NBO;
    std::string object_path, name;

    std::vector <glm::vec3> vertices;
    std::vector <glm::vec3> normals;
    std::vector<Tri> triangle_indicies;
    std::vector <unsigned int> indicies;
    std::vector<UVCoord> uv;

    Material material;

    glm::vec3 bbmin;
    glm::vec3 bbmax;

    glm::mat4 tansform;

    void applyTransform();

    void openglInit();

    void destroy_buffers();
};

#endif
