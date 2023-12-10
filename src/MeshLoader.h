//
// Created by bat on 20/11/23.
//

#ifndef OBJECT_VIEWER_MESHLOADER_H
#define OBJECT_VIEWER_MESHLOADER_H

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>

#include "Mesh.h"
#include "deps/glm/glm.hpp"
#include "Material.h"
#include "stb_image.h"

namespace MeshLoader{

    bool import(const std::string & pFile) {
        // Create an instance of the Importer class
        Assimp::Importer importer;

        // And have it read the given file with some example postprocessing
        // Usually - if speed is not the most important aspect for you - you'll
        // probably to request more postprocessing than we do in this example.
        const aiScene* aiScene = importer.ReadFile(pFile,
                                                  aiProcess_JoinIdenticalVertices |
                                                  aiProcess_Triangulate |
                                                  aiProcess_DropNormals |
                                                  aiProcess_GenSmoothNormals);

        // If the import failed, report it
        if (nullptr == aiScene) {
            std::cout << "Erreur MeshLoader" << std::endl;
            return false;
        }

        int meshNumber = aiScene->mNumMeshes;
        int textureNumber = aiScene->mNumTextures;

        for(int i = 0; i < meshNumber; ++i)
        {
            auto* newMesh = new Mesh();

            aiMesh* aiMesh = aiScene->mMeshes[i];

            // Now we can access the file's contents.
            unsigned int sizeV = aiMesh->mNumVertices;
            unsigned int sizeT = aiMesh->mNumFaces;

            newMesh->vertices.resize (sizeV);
            newMesh->triangle_indicies.resize (sizeT);
            newMesh->indicies.resize (sizeT*3);
            newMesh->normals.resize(sizeV);

            newMesh->name = aiMesh->mName.C_Str();

            for (unsigned int j = 0; j < sizeV; ++j)
            {
                float x = aiMesh->mVertices[j].x;
                float y = aiMesh->mVertices[j].y;
                float z = aiMesh->mVertices[j].z;
                glm::vec3 pos(x,y,z);
                newMesh->vertices[j] = pos;
            }

            for (unsigned int j = 0; j < sizeV; ++j)
            {
                float x = aiMesh->mNormals[j].x;
                float y = aiMesh->mNormals[j].y;
                float z = aiMesh->mNormals[j].z;
                glm::vec3 normal(x,y,z);
                newMesh->normals[j] = normal;
            }

            for (unsigned int j = 0; j < sizeT; ++j) {
                unsigned int ind1 = aiMesh->mFaces[j].mIndices[0];
                unsigned int ind2 = aiMesh->mFaces[j].mIndices[1];
                unsigned int ind3 = aiMesh->mFaces[j].mIndices[2];

                newMesh->triangle_indicies[j].vertices[0] = ind1;
                newMesh->triangle_indicies[j].vertices[1] = ind2;
                newMesh->triangle_indicies[j].vertices[2] = ind3;

                int offset = j * 3;

                newMesh->indicies[offset] = ind1;
                newMesh->indicies[offset + 1] = ind2;
                newMesh->indicies[offset + 2] = ind3;
            }

            aiAABB aabb = aiMesh->mAABB;

            newMesh->bbmin = glm::vec3(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z);
            newMesh->bbmax = glm::vec3(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z);


            // --------------- TEXTURE ---------------- //

            if (aiMesh->HasTextureCoords(0)) {
                // Access texture coordinates


                for(unsigned int texCoord = 0; texCoord < sizeV; ++texCoord)
                {
                    aiVector3D uv = aiMesh->mTextureCoords[0][texCoord];

                    newMesh->uv.emplace_back(uv.x);
                    newMesh->uv.emplace_back(uv.y);
                }

                // Access material
                aiMaterial* material = aiScene->mMaterials[aiMesh->mMaterialIndex];

                // Access the texture count
                unsigned int textureCount = material->GetTextureCount(aiTextureType_DIFFUSE);

                // Loop through all textures of the mesh
                for (unsigned int j = 0; j < textureCount; ++j) {
                    aiString texturePath;
                    material->GetTexture(aiTextureType_DIFFUSE, j, &texturePath);

                    const aiTexture* aiTex = aiScene->GetEmbeddedTexture(texturePath.C_Str());

                    Texture texture;

                    // Si la texture est sous un format d'image compréssé
                    // appelle stb_image.h pour décomprésser
                    if(aiTex->mHeight == 0) {

                        int width, height, channels;

                        unsigned char* imageData = stbi_load_from_memory(
                                reinterpret_cast<unsigned char*>(aiTex->pcData),
                                aiTex->mWidth,
                                &width,
                                &height,
                                &channels,
                                STBI_rgb
                        );

                        if(!imageData){
                            std::cerr << "Failed to load embedded compressed texture\n";
                        }

                        texture.height = height;
                        texture.width = width;

                        for(int y = 0; y < height; ++y)
                        {
                            for(int x = 0; x < width; ++x)
                            {
                                int pos = (x*channels) + (y*channels) * width;
                                texture.data.push_back(imageData[pos]);
                                texture.data.push_back(imageData[pos+1]);
                                texture.data.push_back(imageData[pos+2]);
                            }
                        }

                        stbi_image_free(imageData);
                    }
                    else
                    {
                        texture.height = aiTex->mHeight;
                        texture.width = aiTex->mWidth;

                        for(int y = 0; y < aiTex->mHeight; ++y)
                        {
                            for(int x = 0; x < aiTex->mWidth; ++x)
                            {
                                int pos = x + y * aiTex->mWidth;
                                texture.data.push_back(aiTex->pcData[pos].r);
                                texture.data.push_back(aiTex->pcData[pos].g);
                                texture.data.push_back(aiTex->pcData[pos].b);
                            }
                        }
                    }

                    newMesh->material.diffuse_texture = texture;
                }
            }

            newMesh->openglInit();
            scene_meshes.push_back(newMesh);
        }

        return true;
    }
}

#endif //OBJECT_VIEWER_MESHLOADER_H