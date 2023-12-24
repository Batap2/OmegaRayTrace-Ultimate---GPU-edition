#include <memory>
#include <iostream>

#include "transform.h"
#include "globals.h"
#include "stb_image.h"

void Mesh::destroy_buffers(){
	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &normal_buffer);
	glDeleteBuffers(1, &index_buffer);
}

void Mesh::openglInit()
{
    // ----------------- MESH ---------------- //

    // Transfer mesh data to GPU
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1,&UVBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO and copy vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Bind EBO and copy index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicies.size(), indicies.data(), GL_STATIC_DRAW);

    // Bind normals to layout location 1
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1); // This allows usage of layout location 1 in the vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Bind UV to layout location 2
    glBindBuffer(GL_ARRAY_BUFFER, UVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uv.size(), uv.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2); // This allows usage of layout location 2 in the vertex shader
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // ----------------- TEXTURES ---------------- //

    diffuse_texture_LOC = glGetUniformLocation(shaderprogram, "diffuse_texture");
    float_texture_LOC = glGetUniformLocation(shaderprogram, "float_texture");

    glUniform1i(diffuse_texture_LOC, 0);
    glUniform1i(float_texture_LOC,  1);

    glGenTextures(1, &diffuse_texture_id);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, diffuse_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 material.diffuse_texture.width,
                 material.diffuse_texture.height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 material.diffuse_texture.data.data()
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1, &float_texture_id);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, float_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 material.float_texture.width,
                 material.float_texture.height,
                 0,
                 GL_RGB,
                 GL_FLOAT,
                 gpuOutputImg.data()
    );
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Mesh::change_texture(FloatTexture tex)
{
    //material.diffuse_texture = tex;

    int h,w,c;
    //unsigned char* imageData = stbi_load("data/tex.jpg", &w, &h, &c, 3);

    material.float_texture.width = tex.width;
    material.float_texture.height = tex.height;

    material.float_texture.data = tex.data;

//    for(int y = 0; y < h; ++y)
//    {
//        for(int x = 0; x < w; ++x)
//        {
//            int pos = (x*c) + (y*c) * w;
//            material.diffuse_texture.data.push_back(imageData[pos]);
//            material.diffuse_texture.data.push_back(imageData[pos+1]);
//            material.diffuse_texture.data.push_back(imageData[pos+2]);
//        }
//    }

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, diffuse_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 material.float_texture.width,
                 material.float_texture.height,
                 0,
                 GL_RGB,
                 GL_FLOAT,
                 gpuOutputImg.data()
    );
    glGenerateMipmap(GL_TEXTURE_2D);
}
