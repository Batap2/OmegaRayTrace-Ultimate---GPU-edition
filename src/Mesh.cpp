#include <memory>
#include <iostream>

#include "transform.h"
#include "globals.h"
#include "stb_image.h"

void Mesh::destroy_buffers(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &UVBO);
    glDeleteBuffers(1, &ambiant_bo);
    glDeleteBuffers(1, &diffuse_bo);
    glDeleteBuffers(1, &specular_bo);
    glDeleteBuffers(1, &mra_bo);
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
    glGenBuffers(1,&ambiant_bo);
    glGenBuffers(1,&diffuse_bo);
    glGenBuffers(1,&specular_bo);
    glGenBuffers(1,&mra_bo);
    glGenBuffers(1,&useTexture_bo);

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

    // ----------------- MATERIAL ---------------- //

    send_material_to_shaders();


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
    material.float_texture.width = tex.width;
    material.float_texture.height = tex.height;

    material.float_texture.data = tex.data;

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

void Mesh::send_material_to_shaders()
{
    //-------------------------------- UPDATING THE MATERIALS OPENCL BUFFER ----------------------------------------------//
    materials_array.clear();
    size_t mesh_number = scene_meshes.size();
    for(size_t i = 0; i < mesh_number; i++) {
        std::array<float,13> current_mat_data = scene_meshes[i]->material.getMaterialData();
        for(int j = 0; j < 13;j++)
        {
            float current_mat_elem = current_mat_data[j];
            materials_array.push_back(current_mat_elem);
        }
    }

    materialsBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * materials_array.size(),materials_array.data());

    //---------------------------------------------------------------------------------------------------------------------//

    if(textureBufferAlreadyCreated){
        updateMaterial();
        return;
    }

    // DIFFUSE
    std::vector<glm::vec3> diffuse_vec;
    for(int i = 0; i < vertices.size(); ++i){
        diffuse_vec.push_back(material.diffuse_material);
    }
    glBindBuffer(GL_ARRAY_BUFFER, diffuse_bo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), diffuse_vec.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(3); // This allows usage of layout location 1 in the vertex shader
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    // MRA
    glm::vec3 mra = material.getMRA();

    std::vector<glm::vec3> mra_vec;
    for(int i = 0; i < vertices.size(); ++i){
        mra_vec.push_back(mra);
    }

    glBindBuffer(GL_ARRAY_BUFFER, mra_bo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), mra_vec.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(4); // This allows usage of layout location 1 in the vertex shader
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Send useTexture, isEmissive and isTransparent parameters
    std::vector<glm::vec3> useT_vec;
    for(int i = 0; i < vertices.size(); ++i){
        useT_vec.push_back(vec3(material.useTexture, material.isEmissive, material.isTransparent));
    }
    glBindBuffer(GL_ARRAY_BUFFER, useTexture_bo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), useT_vec.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(5); // This allows usage of layout location 1 in the vertex shader
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    textureBufferAlreadyCreated = true;

    //////////////////////////// SEND UPDATED MATERIALS TO KERNEL.CL ////////////////////////

}

void Mesh::updateMaterial(){
    // DIFFUSE
    std::vector<glm::vec3> diffuse_vec;
    for(int i = 0; i < vertices.size(); ++i){
        diffuse_vec.push_back(material.diffuse_material);
    }
    glBindBuffer(GL_ARRAY_BUFFER, diffuse_bo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertices.size(), diffuse_vec.data());

    // MRA
    glm::vec3 mra = material.getMRA();

    std::vector<glm::vec3> mra_vec;
    for(int i = 0; i < vertices.size(); ++i){
        mra_vec.push_back(mra);
    }

    // USETEX, EMISSIVE, TRANSPARENT
    std::vector<glm::vec3> useT_vec;
    for(int i = 0; i < vertices.size(); ++i){
        useT_vec.push_back(vec3(material.useTexture, material.isEmissive, material.isTransparent));
    }
    glBindBuffer(GL_ARRAY_BUFFER, useTexture_bo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertices.size(), useT_vec.data());


    glBindBuffer(GL_ARRAY_BUFFER, mra_bo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertices.size(), mra_vec.data());


}
