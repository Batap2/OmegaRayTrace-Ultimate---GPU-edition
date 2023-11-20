//
// Created by bat on 20/11/23.
//

#ifndef OBJECT_VIEWER_MESHLOADER_H
#define OBJECT_VIEWER_MESHLOADER_H

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>

#include "mesh.h"
#include "deps/glm/glm.hpp"

namespace ObjLoader{

    bool import(gl::Mesh &mesh, const std::string& pFile) {
        // Create an instance of the Importer class
        Assimp::Importer importer;

        // And have it read the given file with some example postprocessing
        // Usually - if speed is not the most important aspect for you - you'll
        // probably to request more postprocessing than we do in this example.
        const aiScene* scene = importer.ReadFile( pFile,
                                                  aiProcess_JoinIdenticalVertices |
                                                  aiProcess_Triangulate |
                                                  aiProcess_DropNormals |
                                                  aiProcess_GenSmoothNormals);

        // If the import failed, report it
        if (nullptr == scene) {
            std::cout << "Erreur ObjLoader" << std::endl;
            return false;
        }

        // Now we can access the file's contents.
        unsigned int sizeV = scene->mMeshes[0]->mNumVertices;
        unsigned int sizeT = scene->mMeshes[0]->mNumFaces;

        mesh.objectVertices.resize (sizeV);
        //TODO mesh.triangles.resize (sizeT);

        for (unsigned int i = 0; i < sizeV; i++){
            float x = scene->mMeshes[0]->mVertices[i].x;
            float y = scene->mMeshes[0]->mVertices[i].y;
            float z = scene->mMeshes[0]->mVertices[i].z;
            glm::vec3 pos(x,y,z);
            mesh.objectVertices[i] = pos;
        }

        // TODO génère les triangles du mesh
//        for (unsigned int i = 0; i < sizeT; i++) {
//            unsigned int ind1 = scene->mMeshes[0]->mFaces[i].mIndices[0];
//            unsigned int ind2 = scene->mMeshes[0]->mFaces[i].mIndices[1];
//            unsigned int ind3 = scene->mMeshes[0]->mFaces[i].mIndices[2];
//
//            mesh.triangles[i].v[0] = ind1;
//            mesh.triangles[i].v[1] = ind2;
//            mesh.triangles[i].v[2] = ind3;
//        }

        // We're done. Everything will be cleaned up by the importer destructor
        return true;
    }
}

#endif //OBJECT_VIEWER_MESHLOADER_H
