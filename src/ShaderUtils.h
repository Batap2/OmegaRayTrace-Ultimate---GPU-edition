//
// Created by bat on 21/11/23.
//

#ifndef OBJECT_VIEWER_SHADERUTILS_H
#define OBJECT_VIEWER_SHADERUTILS_H


#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "globals.h"


namespace ShaderUtils{

    struct ShaderLight{
        glm::vec3 pos;
        glm::vec3 color;
        float intensity;
    };

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
        if(lights.size() > lightsMaxNumber){
            std::cout << "max lights number reached\n";
            return;
        }

        int structSize = 7;
        GLfloat lightsBuffer[lightsMaxNumber * structSize];


        for(int i = 0; i < lights.size(); ++i)
        {
            Light l = lights[i];

            int offset = i*structSize;

            lightsBuffer[offset] = l.pos.x;
            lightsBuffer[offset+1] = l.pos.y;
            lightsBuffer[offset+2] = l.pos.z;
            lightsBuffer[offset+3] = l.color.x;
            lightsBuffer[offset+4] = l.color.y;
            lightsBuffer[offset+5] = l.color.z;
            lightsBuffer[offset+6] = l.intensity;
        }


        lightsBufferID = glGetUniformLocation(shaderprogram, "lights");
        glUniform1fv(lightsBufferID, lightsMaxNumber * structSize, lightsBuffer);

        lights_numberID = glGetUniformLocation(shaderprogram, "lights_number");
        glUniform1i(lights_numberID, lights.size());
    }
}

#endif //OBJECT_VIEWER_SHADERUTILS_H
