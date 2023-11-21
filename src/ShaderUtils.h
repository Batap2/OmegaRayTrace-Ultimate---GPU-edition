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
        std::vector<glm::vec3> lightsPos;
        std::vector<glm::vec3> lightsColor;

        glGenBuffers(1, &lightsPosID);
        glGenBuffers(1, &lightsColorID);

        glBindBuffer(GL_UNIFORM_BUFFER, lightsPosID);
        glBindBuffer(GL_UNIFORM_BUFFER, lightsColorID);

        glBufferData(GL_ARRAY_BUFFER, lightsPos.size() * sizeof(glm::vec3), lightsPos.data(), GL_DYNAMIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, lightsColor.size() * sizeof(glm::vec3), lightsColor.data(), GL_DYNAMIC_DRAW);

        glBindBufferBase(GL_UNIFORM_BUFFER, lightsPosAttribPos, lightsPosID);
    }
}

#endif //OBJECT_VIEWER_SHADERUTILS_H
