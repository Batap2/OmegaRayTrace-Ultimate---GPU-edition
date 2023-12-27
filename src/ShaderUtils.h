//
// Created by bat on 21/11/23.
//

#ifndef OBJECT_VIEWER_SHADERUTILS_H
#define OBJECT_VIEWER_SHADERUTILS_H


#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "globals.h"
#include "shaders.h"


namespace ShaderUtils{

    struct ShaderLight{
        glm::vec3 pos;
        glm::vec3 color;
        float intensity;
    };

    //TODO  cpt
    void drawLights()
    {

//            glPointSize(5.0f); // Définit la taille du point
//
//            glBegin(GL_POINTS); // Commence à dessiner des points
//            glColor3f(l->color.x, l->color.y, l->color.z); // Couleur du point : blanc
//            glVertex3f(l->pos.x, l->color.y, l->color.z); // Coordonnées du point (ici, le centre de l'écran)
//            glEnd();

        GLuint lightsVertexArray, lightsVertexBuffer;

        glGenVertexArrays(1, &lightsVertexArray);
        glGenBuffers(1, &lightsVertexBuffer);

        glBindVertexArray(lightsVertexArray);

        // Bind vertices to layout location 0
        glBindBuffer(GL_ARRAY_BUFFER, lightsVertexArray );

        std::vector<glm::vec3> lightsVertex;
        for(Light* l : scene_lights){
            lightsVertex.push_back(l->pos);
        }

        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * lightsVertex.size(), &lightsVertex[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(50); // This allows usage of layout location 0 in the vertex shader
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(lightsVertexArray);

        glPointSize(2.5);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glDrawElements(GL_TRIANGLES, scene_lights.size()*3, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

    }

    void init_shaders(){
        // Initialize shaders
        Shader shader;
        vertexshader = shader.init_shaders(GL_VERTEX_SHADER, "res/shaders/vertex.glsl") ;
        fragmentshader = shader.init_shaders(GL_FRAGMENT_SHADER, "res/shaders/fragment.glsl") ;
        shaderprogram = shader.init_program(vertexshader, fragmentshader) ;

        // Get uniform locations
        projectionLoc = glGetUniformLocation(shaderprogram, "projection");
        modelviewLoc = glGetUniformLocation(shaderprogram, "modelview");
        camPosLoc = glGetUniformLocation(shaderprogram, "camPos");
        render_modeLoc = glGetUniformLocation(shaderprogram, "render_mode");


    }

    void reshape(GLFWwindow* window, int width, int height){
        window_width = width;
        window_height = height;

        // vp pour avoir une rendu plus petit
        //int vp = width / screenSeparation1;
        int vp = 0;

        current_vp_height = height;
        current_vp_width = vp;

        glViewport(vp, 0, width - vp, height);

        float aspect = (float) (width - vp) / (float) height;
        projection = glm::perspective(glm::radians(fovy), aspect, zNear, zFar);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    }

    void sendLightsToShaders()
    {
        if(scene_lights.size() > lightsMaxNumber){
            std::cout << "max scene_lights number reached\n";
            return;
        }

        int structSize = 7;
        GLfloat lightsBuffer[lightsMaxNumber * structSize];


        for(int i = 0; i < scene_lights.size(); ++i)
        {
            Light* l = scene_lights[i];

            int offset = i*structSize;

            lightsBuffer[offset] = l->pos.x;
            lightsBuffer[offset+1] = l->pos.y;
            lightsBuffer[offset+2] = l->pos.z;
            lightsBuffer[offset+3] = l->color.x;
            lightsBuffer[offset+4] = l->color.y;
            lightsBuffer[offset+5] = l->color.z;
            lightsBuffer[offset+6] = l->intensity;
        }

        lightsBufferID = glGetUniformLocation(shaderprogram, "lights");
        lights_numberID = glGetUniformLocation(shaderprogram, "lights_number");
        glUniform1fv(lightsBufferID, lightsMaxNumber * structSize, lightsBuffer);
        glUniform1i(lights_numberID, scene_lights.size());
    }

    void sendMaterialsToShader()
    {
        if(scene_lights.size() > lightsMaxNumber){
            std::cout << "max scene_lights number reached\n";
            return;
        }

        // struct :
        // [ambiant_color, diffuse_color, specular_color, shininess, metallic, roughness, ao]
        // [3,3,3,1,1,1,1]
        int structSize = 13;
        GLfloat materialBuffer[objectMaxNumber * structSize];

        for(int i = 0; i < scene_meshes.size(); ++i)
        {
            Material m = scene_meshes[i]->material;

            int offset = i*structSize;

            materialBuffer[offset] = m.ambient_material[0];
            materialBuffer[offset+1] = m.ambient_material[1];
            materialBuffer[offset+2] = m.ambient_material[2];

            materialBuffer[offset+3] = m.diffuse_material[0];
            materialBuffer[offset+4] = m.diffuse_material[1];
            materialBuffer[offset+5] = m.diffuse_material[2];

            materialBuffer[offset+6] = m.specular_material[0];
            materialBuffer[offset+7] = m.specular_material[1];
            materialBuffer[offset+8] = m.specular_material[2];

            materialBuffer[offset+9] = m.shininess;
            materialBuffer[offset+10] = m.metallic;
            materialBuffer[offset+11] = m.roughness;
            materialBuffer[offset+12] = m.ao;
        }


        materialBufferID = glGetUniformLocation(shaderprogram, "materials");
        glUniform1fv(materialBufferID, objectMaxNumber * structSize, materialBuffer);

        lights_numberID = glGetUniformLocation(shaderprogram, "materials_number");
        glUniform1i(objectNumberID, scene_meshes.size());
    }
}

#endif //OBJECT_VIEWER_SHADERUTILS_H
