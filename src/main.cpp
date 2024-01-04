#define STB_IMAGE_IMPLEMENTATION

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
#include "ClFunctions.h"

#include "shaders.h"
#include "transform.h"
#include "globals.h"

#include <CL/cl.h>
#include <CL/cl2.hpp>
#include "CL/cl_gl.h"




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
            case GLFW_KEY_Y:
                mainCamera.cameraPos = eyeinit ;
                mainCamera.cameraUp = upinit ;
                amount = amountinit ;
                transop = view2 ;
                sx = sy = 1.0 ; 
                tx = ty = 0.0 ;
                updateCL_Camera(window_width,window_height,clQueue,clProgram,devices[0]);

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
            case GLFW_KEY_T:
                render_mode = (render_mode + 1)%2;
                glUniform1i(render_modeLoc, render_mode);
                break;
            case GLFW_KEY_P:
                flat_screen.change_texture(FloatTexture());
                break;
            case GLFW_KEY_R:
                cl::Buffer buffer(clContext, CL_MEM_READ_WRITE, gpuOutputImg_size);
                renderImage(buffer, (int)window_width, (int)window_height, clQueue, clProgram, devices);
                clQueue.enqueueReadBuffer(buffer, CL_TRUE, 0, gpuOutputImg.size() * sizeof(float), gpuOutputImg.data());
                writeImageToFile(gpuOutputImg, gpuOutputImg_size, (int)window_width, (int)window_height);
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
        updateCL_Camera(window_width,window_height,clQueue,clProgram,devices[0]);

        render_number = 0;

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
        render_number = 0;
        if(leftHolded){
            mainCamera.cameraPos -= cameraSpeed * mainCamera.cameraRight;
            updateCL_Camera(window_width,window_height,clQueue,clProgram,devices[0]);

        }
        if(rightHolded){
            mainCamera.cameraPos += cameraSpeed * mainCamera.cameraRight;
            updateCL_Camera(window_width,window_height,clQueue,clProgram,devices[0]);

        }
        if(upHolded){
            mainCamera.cameraPos += cameraSpeed * mainCamera.cameraDirection;
            updateCL_Camera(window_width,window_height,clQueue,clProgram,devices[0]);

        }
        if(downHolded){
            mainCamera.cameraPos -= cameraSpeed * mainCamera.cameraDirection;
            updateCL_Camera(window_width,window_height,clQueue,clProgram,devices[0]);

        }
        if(aHolded){
            mainCamera.cameraPos -= cameraSpeed * upinit;
            updateCL_Camera(window_width,window_height,clQueue,clProgram,devices[0]);

        }
        if(eHolded){
            mainCamera.cameraPos += cameraSpeed * upinit;
            updateCL_Camera(window_width,window_height,clQueue,clProgram,devices[0]);

        }
    }
}

void display() {
    glClearColor(skyColor.x, skyColor.y, skyColor.z, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(render_mode == 0)
    {
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
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, meshP->diffuse_texture_id);
            glDrawElements(GL_TRIANGLES, meshP->indicies.size(), GL_UNSIGNED_INT, 0);
        }
    } else
    {
        cl::Buffer buffer(clContext, CL_MEM_READ_WRITE, gpuOutputImg_size);
        renderImage(buffer, window_width, window_height, clQueue, clProgram, devices);
        clQueue.enqueueReadBuffer(buffer, CL_TRUE, 0, gpuOutputImg.size() * sizeof(float), gpuOutputImg.data());

        render_number++;

        ShaderUtils::concatRender(gpuOutputImg);

        FloatTexture newTex;
        newTex.height = window_height;
        newTex.width = window_width;
        newTex.data = actual_render;

        flat_screen.change_texture(newTex);

        glBindVertexArray(flat_screen.VAO);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, flat_screen.float_texture_id);
        glDrawElements(GL_TRIANGLES, flat_screen.indicies.size(), GL_UNSIGNED_INT, 0);
    }


}

int main(int argc, char* argv[]){

//---------------------------- GPU PART ----------------------------------//

    initializeOpenCL(clContext, clQueue, clProgram, gpuOutputImg, gpuOutputImg_size, window_width, window_width, devices);


    //-------------------------------------------------------------//

    // Initialise GLFW and GLEW;
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
    SceneOperations::openFile("data/plane2.fbx");
    SceneOperations::init_flat_screen();

    ShaderUtils::reshape(window, window_width, window_height);
    ShaderUtils::sendLightsToShaders();

    mainCamera.cameraPos = vec3(0,0.2,4);

    rng.seed(std::random_device()());

    initializeBuffers();
    load(window_width,window_height,clQueue,clProgram,devices[0]);


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
    glfwTerminate();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
