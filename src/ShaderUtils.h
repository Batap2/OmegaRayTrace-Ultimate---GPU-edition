//
// Created by bat on 21/11/23.
//

#ifndef OBJECT_VIEWER_SHADERUTILS_H
#define OBJECT_VIEWER_SHADERUTILS_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "globals.h"


namespace ShaderUtils{
    void reshape(GLFWwindow* window, int width, int height){
        window_width = width;
        window_height = height;

        int vp = width / screenSeparation1;

        current_vp_height = height;
        current_vp_width = vp;

        glViewport(vp, 0, width - vp, height);

        float aspect = (float) (width - vp) / (float) height;
        projection = glm::perspective(glm::radians(fovy), aspect, zNear, zFar);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    }

    void sendLightsToShaders(){
        std::vector<glm::vec3> lightsPosBuffer;
        std::vector<glm::vec3> lightsColorBuffer;

        lightsPosBuffer.push_back(glm::vec3(0.0f, 10.0f, 0.0f)); // Ajoutez vos positions de lumière
        lightsColorBuffer.push_back(glm::vec3(1.0f, 1.0f, 1.0f)); // Ajoutez vos couleurs de lumière

        glGenBuffers(1, &lightsPosBufferID);
        glGenBuffers(1, &lightsColorBufferID);

        glBindBuffer(GL_UNIFORM_BUFFER, lightsPosBufferID);
        glBufferData(GL_UNIFORM_BUFFER, lightsPosBuffer.size() * sizeof(glm::vec3), lightsPosBuffer.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_UNIFORM_BUFFER, lightsColorBufferID);
        glBufferData(GL_UNIFORM_BUFFER, lightsColorBuffer.size() * sizeof(glm::vec3), lightsColorBuffer.data(), GL_DYNAMIC_DRAW);

        GLuint lightsPosBindingIndex = glGetUniformBlockIndex(shaderprogram, "LightsPosBuffer");
        GLuint lightsColorBindingIndex = glGetUniformBlockIndex(shaderprogram, "LightsColorBuffer");

        glBindBufferBase(GL_UNIFORM_BUFFER, lightsPosBindingIndex, lightsPosBufferID);
        glBindBufferBase(GL_UNIFORM_BUFFER, lightsColorBindingIndex, lightsColorBufferID);
    }
}

#endif //OBJECT_VIEWER_SHADERUTILS_H
