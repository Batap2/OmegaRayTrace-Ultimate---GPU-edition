#include <memory>
#include <iostream>

#include "transform.h"
#include "globals.h"

void Mesh::destroy_buffers(){
	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &normal_buffer);
	glDeleteBuffers(1, &index_buffer);
}

void Mesh::openglInit()
{
    // Transfer mesh data to GPU
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &EBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO and copy vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Bind normals to layout location 1
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1); // This allows usage of layout location 1 in the vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Bind EBO and copy index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicies.size(), indicies.data(), GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
