#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "deps/glm/ext.hpp"

#include "deps/imgui/imgui.h"
#include "deps/imgui/imgui_impl_glfw.h"
#include "deps/imgui/imgui_impl_opengl3.h"


#include "Mesh.h"
#include "camera.h"
#include "GUI.h"
#include "ShaderUtils.h"
#include "SceneOperations.h"

#include "shaders.h"
#include "transform.h"
#include "globals.h"

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        rclickHolded = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
        rclickHolded = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS){
        switch(key){
            case GLFW_KEY_ESCAPE: 
                exit(0) ;
                break ;
            case GLFW_KEY_R:
                mainCamera.cameraPos = eyeinit ;
                mainCamera.cameraUp = upinit ;
                amount = amountinit ;
                transop = view2 ;
                sx = sy = 1.0 ; 
                tx = ty = 0.0 ; 
                break ;
            case GLFW_KEY_A:
                leftHolded = true;
                break;
            case GLFW_KEY_D:
                rightHolded = true;
                break;
            case GLFW_KEY_W:
                upHolded = true;
                break;
            case GLFW_KEY_S:
                 downHolded = true;
                break;
            case GLFW_KEY_Q:
                aHolded = true;
                break;
            case GLFW_KEY_E:
                eHolded = true;
                break;
            case GLFW_KEY_TAB:
                showMenus = !showMenus;
                break;
        }
    } else if (action == GLFW_RELEASE)
    {
        switch(key){
            case GLFW_KEY_A:
                leftHolded = false;
                break;
            case GLFW_KEY_D:
                rightHolded = false;
                break;
            case GLFW_KEY_W:
                upHolded = false;
                break;
            case GLFW_KEY_S:
                downHolded = false;
                break;
            case GLFW_KEY_Q:
                aHolded = false;
                break;
            case GLFW_KEY_E:
                eHolded = false;
                break;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(rclickHolded)
    {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw   += xoffset;
        pitch += yoffset;

        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        mainCamera.cameraDirection = glm::normalize(direction);
        mainCamera.cameraRight = glm::normalize(glm::cross(mainCamera.cameraDirection, upinit));
        mainCamera.cameraUp = glm::cross(mainCamera.cameraRight, mainCamera.cameraDirection);
    } else {
        lastX = xpos;
        lastY = ypos;
    }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    cameraSpeed += yoffset * 0.005;
    cameraSpeed = fmax(0.0f, cameraSpeed);
}

void manageInput()
{
    if(rclickHolded)
    {
        if(leftHolded){
            mainCamera.cameraPos -= cameraSpeed * mainCamera.cameraRight;
        }
        if(rightHolded){
            mainCamera.cameraPos += cameraSpeed * mainCamera.cameraRight;
        }
        if(upHolded){
            mainCamera.cameraPos += cameraSpeed * mainCamera.cameraDirection;
        }
        if(downHolded){
            mainCamera.cameraPos -= cameraSpeed * mainCamera.cameraDirection;
        }
        if(aHolded){
            mainCamera.cameraPos -= cameraSpeed * upinit;
        }
        if(eHolded){
            mainCamera.cameraPos += cameraSpeed * upinit;
        }
    }
}

void display() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    modelview = glm::lookAt(mainCamera.cameraPos,mainCamera.cameraPos + mainCamera.cameraDirection, mainCamera.cameraUp);

    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, &modelview[0][0]);

    glUniform3fv(camPosLoc, 1, glm::value_ptr(mainCamera.cameraPos));

    const GLfloat white[4] = {1, 1, 1, 1};
    const GLfloat black[4] = {0, 0, 0, 0};

    glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, &modelview[0][0]);

    for(Mesh* meshP : scene_meshes)
    {
        glBindVertexArray(meshP->VAO);

        glDrawElements(GL_TRIANGLES, meshP->indicies.size(), GL_UNSIGNED_INT, 0);
    }

}

int main(int argc, char* argv[]){
    // Initialise GLFW and GLEW; and parse path from command line  
    GLFWwindow* window;

    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    window = glfwCreateWindow(window_width, window_height, "Scene Viewer", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) std::cout << "ERROR : glewInit\n ";
    std::cout << glGetString(GL_VERSION) << "\n"; 

    // Initialize ImGui 
    const char* glsl_version = "#version 330";
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true); 
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    // Initialise all variable initial values
    ShaderUtils::init_shaders();

    glEnable(GL_DEPTH_TEST);
    glfwSetWindowSizeCallback(window, ShaderUtils::reshape);

    glfwSetKeyCallback(window, keyboard);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    SceneOperations::initSceneLights();

    ShaderUtils::reshape(window, window_width, window_height);
    ShaderUtils::sendLightsToShaders();

    while (!glfwWindowShouldClose(window))
    {
        manageInput();

        display();

        if(showMenus){
            GUI::draw(window);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    SceneOperations::destroyScene();
    mesh.destroy_buffers();
    glfwTerminate();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}