#ifndef MESH_H
#define MESH_H

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

#include "deps/glm/glm.hpp"


class Mesh{
private:
    GLuint vertex_array, vertex_buffer, normal_buffer, index_buffer;
public:
    std::string object_path; 

    std::vector <glm::vec3> objectVertices;
    std::vector <glm::vec3> objectNormals;
    std::vector <unsigned int> objectIndices;
    //std::vector <>

    glm::mat4 tansform;

    void generate_buffers();
    void destroy_buffers();
    void parse_and_bind();
    inline void bind(){glBindVertexArray(vertex_array);}
};

#endif
