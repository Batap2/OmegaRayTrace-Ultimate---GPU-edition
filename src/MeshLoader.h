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

        for(int i = 0; i < meshNumber; ++i)
        {
            auto* newMesh = new Mesh();

            // Now we can access the file's contents.
            unsigned int sizeV = aiScene->mMeshes[i]->mNumVertices;
            unsigned int sizeT = aiScene->mMeshes[i]->mNumFaces;

            newMesh->vertices.resize (sizeV);
            newMesh->triangle_indicies.resize (sizeT);
            newMesh->indicies.resize (sizeT*3);
            newMesh->normals.resize(sizeV);

            newMesh->name = aiScene->mName.C_Str();

            for (unsigned int j = 0; j < sizeV; ++j)
            {
                float x = aiScene->mMeshes[i]->mVertices[j].x;
                float y = aiScene->mMeshes[i]->mVertices[j].y;
                float z = aiScene->mMeshes[i]->mVertices[j].z;
                glm::vec3 pos(x,y,z);
                newMesh->vertices[j] = pos;
            }

            for (unsigned int j = 0; j < sizeV; ++j)
            {
                float x = aiScene->mMeshes[i]->mNormals[j].x;
                float y = aiScene->mMeshes[i]->mNormals[j].y;
                float z = aiScene->mMeshes[i]->mNormals[j].z;
                glm::vec3 normal(x,y,z);
                newMesh->normals[j] = normal;
            }

            for (unsigned int j = 0; j < sizeT; ++j) {
                unsigned int ind1 = aiScene->mMeshes[i]->mFaces[j].mIndices[0];
                unsigned int ind2 = aiScene->mMeshes[i]->mFaces[j].mIndices[1];
                unsigned int ind3 = aiScene->mMeshes[i]->mFaces[j].mIndices[2];

                newMesh->triangle_indicies[j].vertices[0] = ind1;
                newMesh->triangle_indicies[j].vertices[1] = ind2;
                newMesh->triangle_indicies[j].vertices[2] = ind3;

                int offset = j * 3;

                newMesh->indicies[offset] = ind1;
                newMesh->indicies[offset + 1] = ind2;
                newMesh->indicies[offset + 2] = ind3;
            }

            aiAABB aabb = aiScene->mMeshes[i]->mAABB;

            newMesh->bbmin = glm::vec3(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z);
            newMesh->bbmax = glm::vec3(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z);

            newMesh->openglInit();
            scene_meshes.push_back(newMesh);
        }


        // We're done. Everything will be cleaned up by the importer destructor
        return true;
    }
}

#endif //OBJECT_VIEWER_MESHLOADER_H
