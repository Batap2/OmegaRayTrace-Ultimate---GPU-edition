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
        
    }
}

#endif //OBJECT_VIEWER_SHADERUTILS_H
