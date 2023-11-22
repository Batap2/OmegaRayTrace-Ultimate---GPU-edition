#ifndef MESH_H
#define MESH_H

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

#include "deps/glm/glm.hpp"

struct UVCoord{
    float u, v;
    UVCoord(float _u, float _v) : u(_u), v(_v) {}
    UVCoord() : u(0.0f), v(0.0f) {}
};

class Mesh{
private:
    GLuint vertex_array, vertex_buffer, normal_buffer, index_buffer;
public:
    std::string object_path; 

    std::vector <glm::vec3> vertices;
    std::vector <glm::vec3> normals;
    std::vector <unsigned int> indicies;
    std::vector<UVCoord> uv;

    glm::mat4 tansform;

    void applyTransform();

    void generate_buffers();
    void destroy_buffers();
    void parse_and_bind();
    inline void bind(){glBindVertexArray(vertex_array);}
};

#endif
