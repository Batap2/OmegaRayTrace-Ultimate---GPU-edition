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

#include "shaders.h"
#include "transform.h"
#include "globals.h"


#include <CL/cl.h>
#include <CL/cl2.hpp>
#include "CL/cl_gl.h"


void extractMeshData(const Mesh& current_mesh, float* outVertices, unsigned int* outIndices, int& outNumTriangles) {
    outNumTriangles = current_mesh.triangle_indicies.size();

    for (size_t i = 0; i < current_mesh.vertices.size(); ++i) {
        outVertices[i * 3] = current_mesh.vertices[i].x;
        outVertices[i * 3 + 1] = current_mesh.vertices[i].y;
        outVertices[i * 3 + 2] = current_mesh.vertices[i].z;
    }

    // Extract indices
    for (size_t i = 0; i < current_mesh.indicies.size(); ++i) {
        outIndices[i * 3] = current_mesh.indicies[i];
        outIndices[i * 3 + 1] = current_mesh.indicies[i+1];
        outIndices[i * 3 + 2] = current_mesh.indicies[i+2];
    }
}

// Fonction pour afficher les triplets de vertices composant chaque triangle
void printTriangles(const float* vertices, const unsigned int* indices, int numTriangles) {
    for (int i = 0; i < numTriangles; ++i) {
        int baseIndex = i * 3;
        int vIndex1 = indices[baseIndex];
        int vIndex2 = indices[baseIndex + 1];
        int vIndex3 = indices[baseIndex + 2];

        std::cout << "Triangle " << i + 1 << ":\n";
        std::cout << "Vertex 1: (" << vertices[vIndex1 * 3] << ", " << vertices[vIndex1 * 3 + 1] << ", " << vertices[vIndex1 * 3 + 2] << ")\n";
        std::cout << "Vertex 2: (" << vertices[vIndex2 * 3] << ", " << vertices[vIndex2 * 3 + 1] << ", " << vertices[vIndex2 * 3 + 2] << ")\n";
        std::cout << "Vertex 3: (" << vertices[vIndex3 * 3] << ", " << vertices[vIndex3 * 3 + 1] << ", " << vertices[vIndex3 * 3 + 2] << ")\n";
        std::cout << "\n";
    }
}

void printVertices(const float* vertices, int numVertices) {
    for (int i = 0; i < numVertices; i++) {
        std::cout << "Vertex " << i << ": (" << vertices[i*3+0] << ", " << vertices[i*3+1] << ", " << vertices[i*3+2] << ")\n";
    }
}

//Fct qui appelle un programme de rendu en gérant les threads pour un device donné
void render(cl::Buffer &buffer, int max_x, int max_y, cl::CommandQueue &queue, cl::Program &program, const cl::Device &device) {

    //Envoi de la camera au GPU
    cl_float* cameraData = mainCamera.getCameraData();
    cl::Buffer cameraBuffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * 9, cameraData);
    queue.enqueueWriteBuffer(cameraBuffer, CL_TRUE, 0, sizeof(cl_float) * 9, cameraData);

    //Envoi des tab indices, vertices et nombre de tri au GPU
    Mesh* meshTest = scene_objects[0]->mesh;
    float* vertices = new float[meshTest->vertices.size() * 3];
    unsigned int* indices = new unsigned int[meshTest->indicies.size() *3];
    int numTriangles = 0;
    cl::Buffer vertexBuffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * meshTest->vertices.size() * 3, vertices);
    cl::Buffer indexBuffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * meshTest->indicies.size() * 3, indices);

    extractMeshData(*meshTest, vertices, indices,numTriangles);

    cl::Kernel kernel(program, "render");
    kernel.setArg(0, buffer);
    kernel.setArg(1, max_x);
    kernel.setArg(2, max_y);
    kernel.setArg(3,cameraBuffer);
    kernel.setArg(4, vertexBuffer);  // Ajouter le buffer de vertices
    kernel.setArg(5, indexBuffer);   // Ajouter le buffer d'indices
    kernel.setArg(6, numTriangles);   // Ajouter le buffer d'indices

    cl::NDRange global(max_x, max_y);
    cl::NDRange local(8, 8);

    queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
    cl_int kernelError = queue.finish();
    if (kernelError != CL_SUCCESS) {
        std::cerr << "OpenCL kernel execution error: " << kernelError << std::endl;
        exit(1);
    }
}

// Fct pour init l'environnement OPENCL et pour charger le programme de rendu
void initializeOpenCL(cl::Context& context, cl::CommandQueue& queue, cl::Program& program, std::vector<float>& img_buffer, size_t& img_buffer_size, int nx, int ny, std::vector<cl::Device>& devices) {
    // Initialize OpenCL
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    if (platforms.empty()) {
        std::cerr << "No OpenCL platform found." << std::endl;
        exit(1);
    }

    cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
    context = cl::Context(CL_DEVICE_TYPE_GPU, properties);

//    cl_context_properties props[ ] =
//            {
//                    CL_GL_CONTEXT_KHR, (cl_context_properties) glfwGetCurrentContext( ),
//                    CL_WGL_HDC_KHR, (cl_context_properties) glX,
//                    CL_CONTEXT_PLATFORM, (cl_context_properties) Platform,
//                    0
//            };
//    cl_context Context = clCreateContext( props, 1, &Device, NULL, NULL, &status );
//    PrintCLError( status, "clCreateContext: " );

    devices = context.getInfo<CL_CONTEXT_DEVICES>();

    if (devices.empty()) {
        std::cerr << "No OpenCL devices found." << std::endl;
        exit(1);
    }

    queue = cl::CommandQueue(context, devices[0]);

    // Create buffer
    img_buffer_size = 3 * nx * ny * sizeof(float);
    img_buffer.resize(3 * nx * ny, 0.0f);

    // Load and compile OpenCL program
    std::ifstream kernelFile("src/kernel.cl");
    if (!kernelFile.is_open()) {
        std::cerr << "Failed to open kernel file. You need to be in root folder" << std::endl;
        exit(1);
    }

    std::string kernelSource((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
    std::vector<std::string> sources;
    sources.push_back(kernelSource);

    cl::Program::Sources clSources;
    clSources.push_back({sources[0].c_str(), sources[0].length()});

    program = cl::Program(context, clSources);

    program.build(devices);
}

//Fct qui appelle render et qui mesure le temps de rendu
void renderImage(cl::Buffer& buffer, int nx, int ny, cl::CommandQueue& queue, cl::Program& program, const std::vector<cl::Device>& devices) {
    // Measure the time before rendering
    clock_t start_time = clock();

    // Render
    render(buffer, nx, ny, queue, program, devices[0]);

    // Measure the time after rendering
    clock_t end_time = clock();
    double duration = double(end_time - start_time) / CLOCKS_PER_SEC;

    std::cerr << "Rendering time: " << duration << " seconds" << std::endl;
}

void writeImageToFile(const std::vector<float>& fb, size_t fb_size, int nx, int ny) {
    // Output FB as Image
    std::ofstream imageFile("ImageRendered.ppm");
    if (!imageFile.is_open()) {
        std::cerr << "Failed to open image file." << std::endl;
        exit(1);
    }

    imageFile << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            size_t pixel_index = j * 3 * nx + i * 3;
            float r = fb[pixel_index + 0] * 255.0f;
            float g = fb[pixel_index + 1] * 255.0f;
            float b = fb[pixel_index + 2] * 255.0f;
            int ir = static_cast<int>(r);
            int ig = static_cast<int>(g);
            int ib = static_cast<int>(b);
            imageFile << ir << " " << ig << " " << ib << "\n";
            //std::cerr << r<< " " << g << " " << b << std::endl;
        }
    }

    imageFile.close();
}

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

        FloatTexture newTex;
        newTex.height = window_height;
        newTex.width = window_width;
        newTex.data = gpuOutputImg;

        flat_screen.change_texture(newTex);

        glBindVertexArray(flat_screen.VAO);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, flat_screen.float_texture_id);
        glDrawElements(GL_TRIANGLES, flat_screen.indicies.size(), GL_UNSIGNED_INT, 0);
    }


}

int main(int argc, char* argv[]){

//---------------------------- GPU PART ----------------------------------//

    initializeOpenCL(clContext, clQueue, clProgram, gpuOutputImg, gpuOutputImg_size, window_width, window_height, devices);

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
    SceneOperations::openFile("data/scene_color.fbx");
    SceneOperations::init_flat_screen();

    ShaderUtils::reshape(window, window_width, window_height);
    ShaderUtils::sendLightsToShaders();

    std::vector<Mesh*> scene_meshes_check = scene_meshes;


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
