#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "deps/glm/ext.hpp"

#include "deps/imgui/imgui.h"
#include "deps/imgui/imgui_impl_glfw.h"
#include "deps/imgui/imgui_impl_opengl3.h"


#include "mesh.h"
#include "camera.h"
#include "GUI.h"
#include "ShaderUtils.h"

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

void initialise_shader_and_mesh(){
    // Initialize shaders
    gl::Shader shader; 
    vertexshader = shader.init_shaders(GL_VERTEX_SHADER, "res/shaders/vertex.glsl") ;
    fragmentshader = shader.init_shaders(GL_FRAGMENT_SHADER, "res/shaders/fragment.glsl") ;
    shaderprogram = shader.init_program(vertexshader, fragmentshader) ;

    // Get uniform locations 
    lightpos = glGetUniformLocation(shaderprogram,"light_posn") ;       
    lightcol = glGetUniformLocation(shaderprogram,"light_col") ;       
    ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;       
    diffusecol =  glGetUniformLocation(shaderprogram,"diffuse") ;       
    specularcol = glGetUniformLocation(shaderprogram,"specular") ;       
    emissioncol = glGetUniformLocation(shaderprogram,"emission") ;       
    shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;
    projectionLoc = glGetUniformLocation(shaderprogram, "projection");
    modelviewLoc = glGetUniformLocation(shaderprogram, "modelview");
    camPosLoc = glGetUniformLocation(shaderprogram, "camPos");

    // Initialize global mesh
    mesh.generate_buffers();
    mesh.parse_and_bind();   
}

void display(float& ambient_slider, float& diffuse_slider, float& specular_slider, float& shininess_slider, bool custom_color, float& light_position, float& light_color) {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    modelview = glm::lookAt(mainCamera.cameraPos,mainCamera.cameraPos + mainCamera.cameraDirection, mainCamera.cameraUp);

    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, &modelview[0][0]);

    glUniform3fv(camPosLoc, 1, glm::value_ptr(mainCamera.cameraPos));

    glUniform4fv(lightpos, numLights, &light_position);
    glUniform4fv(lightcol, numLights, &light_color);  

    // Transformations for objects, involving translation and scaling 
    mat4 sc(1.0) , tr(1.0), transf(1.0); 
    sc = gl::Transform::scale(sx,sy,1.0); 
    tr = gl::Transform::translate(tx,ty,0.0);   
    modelview = tr * sc * modelview;

    if (!custom_color){
        *(&ambient_slider + 1) = ambient_slider;
        *(&ambient_slider + 2) = ambient_slider; 

        *(&diffuse_slider + 1) = diffuse_slider;
        *(&diffuse_slider + 2) = diffuse_slider; 

        *(&specular_slider + 1) = specular_slider;
        *(&specular_slider + 2) = specular_slider; 
    }

    glUniform4fv(ambientcol, 1, &ambient_slider);
    glUniform4fv(diffusecol, 1, &diffuse_slider);
    glUniform4fv(specularcol, 1, &specular_slider);
    glUniform1f(shininesscol, shininess_slider);

    const GLfloat white[4] = {1, 1, 1, 1};
    const GLfloat black[4] = {0, 0, 0, 0};

    glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, &modelview[0][0]);
    mesh.bind();
    if (render_mode == 0){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, mesh.objectIndices.size(), GL_UNSIGNED_INT, 0);
    }
    if (render_mode == 1){
        glLineWidth(1); 
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, mesh.objectIndices.size(), GL_UNSIGNED_INT, 0);
    } 
    if (render_mode == 2){
        glPointSize(2.5); 
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glDrawElements(GL_TRIANGLES, mesh.objectIndices.size(), GL_UNSIGNED_INT, 0);
    } 
    if (render_mode == 3){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, mesh.objectIndices.size(), GL_UNSIGNED_INT, 0);
        glUniform4fv(diffusecol, 1, black);
        glUniform4fv(specularcol, 1, white);

        glPointSize(2.5); 
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glDrawElements(GL_TRIANGLES, mesh.objectIndices.size(), GL_UNSIGNED_INT, 0);

        glLineWidth(2.5); 
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, mesh.objectIndices.size(), GL_UNSIGNED_INT, 0);
    } 
    glBindVertexArray(0);
}

int main(int argc, char* argv[]){
    // Initialise GLFW and GLEW; and parse path from command line  
    GLFWwindow* window;
    mesh.object_path = argv[1]; 

    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    window = glfwCreateWindow(1920, 1080, "Scene Viewer", NULL, NULL);
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
    initialise_shader_and_mesh();
    eye = (eyeinit); 
    up = (upinit); 
    amount = amountinit;
    sx = sy = 1.0;   
    tx = ty = 0.0;

    Camera camera = Camera();

    glEnable(GL_DEPTH_TEST);
    glfwSetWindowSizeCallback(window, ShaderUtils::reshape);

    glfwSetKeyCallback(window, keyboard);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    float ambient[4] = {0.05, 0.05, 0.05, 1};
    float diffusion[4] = {0.2, 0.2, 0.2, 1};
    float specular[4] = {0.5, 0.5, 0.5, 1};
    float light_positions[20] = {0.0f};
    float light_colors[20] = {0.0f};
    float shininess = 10; 
    bool custom_color = false; 
    
    light_positions[1] = 6.5f;
    light_colors[0] = 1.0f;
    light_colors[1] = 1.0f;
    light_colors[2] = 1.0f;

    for (int i = 0; i < numLights; i++){
        light_positions[(4 * i) + 3] = 1.0f;
        light_colors[(4 * i) + 3] = 1.0f;
    }

    previous_y_position = 0;
    previous_x_position = 0;
    render_mode = 3;

    current_vp_width = 1920/4;
    current_vp_height = 1080;

    while (!glfwWindowShouldClose(window))
    {
        manageInput();

        display(*ambient, *diffusion, *specular, shininess, custom_color, *light_positions, *light_colors);
        GUI::draw(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    mesh.destroy_buffers();
    glfwTerminate();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
