//
// Created by mylgasia on 28/12/23.
//

#ifndef OBJECT_VIEWER_CLFUNCTIONS_H
#define OBJECT_VIEWER_CLFUNCTIONS_H

#include "globals.h"
#include <CL/cl.h>
#include <CL/cl2.hpp>
#include "CL/cl_gl.h"


// Function to check if a glm::vec3 already exists in a vector
bool vec3Exists(const std::vector<glm::vec3>& vec, const glm::vec3& target)
{
    for (const auto& element : vec)
    {
        if (element == target)
        {
            return true; // Found the glm::vec3 in the vector
        }
    }
    return false; // glm::vec3 not found in the vector
}

// Function to push a glm::vec3 into a vector only if it doesn't already exist
bool pushBackIfNotExists(std::vector<glm::vec3>& vec, const glm::vec3& element)
{
    if (!vec3Exists(vec, element))
    {
        vec.push_back(element);
        return true; // Successfully pushed
    }
    return false; // Element already exists, not pushed
}


void convertVec3ToFloat(const std::vector<glm::vec3>& vec3Vector, std::vector<float>& floatVector)
{
    for (const auto& vec3 : vec3Vector)
    {
        floatVector.push_back(vec3.x);
        floatVector.push_back(vec3.y);
        floatVector.push_back(vec3.z);
    }
}


void updateMaterialBuffer()
{
    materials_array.clear();
    size_t mesh_number = scene_meshes.size();
    for(size_t i = 0; i < mesh_number; i++) {
        std::array<float,13> current_mat_data = scene_meshes[i]->material.getMaterialData();
        for(int j = 0; j < 13;j++)
        {
            float current_mat_elem = current_mat_data[j];
            materials_array.push_back(current_mat_elem);
        }
    }
    std::cout<<"Number of elements in materials array : "<< materials_array.size()<<std::endl;
    std::cout<<"Number of meshes by using materials array : "<< materials_array.size() / 13 <<std::endl;
    materialsBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * materials_array.size(),materials_array.data());

}

void updateskyColorBuffer()
{
    skyColorBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * 3, &skyColor);
}

void extractSceneData()
{
    vertices_array.clear();
    indices_array.clear();
    splitMesh_array.clear();
    splitMeshTri_array.clear();

    size_t mesh_number = scene_meshes.size();
    std::vector<glm::vec3> vertexChecklist;
    std::cout<<"Nbr of meshes in the scene : "<< mesh_number<<"\n";
    meshNbr = (int)mesh_number;
    for(size_t i = 0; i < mesh_number; i++)
    {
        Mesh m = *scene_meshes[i];
        size_t n_tri = m.triangle_indicies.size();
        size_t n_id = m.triangle_indicies.size()*3;
        size_t n_vertex_float = m.vertices.size()*3;
        splitMeshTri_array.push_back(n_tri);

        std::cout<<"Mesh: "<< i+1 <<"\n";
        std::cout<<" Number of triangles : "<< n_tri<<std::endl;
        std::cout<<" Number of indices : "<< n_id <<std::endl;
        std::cout<<" Number of vertex float : "<< n_vertex_float<<std::endl;

        unsigned int element_pushed =0;
        for (size_t tri = 0; tri < n_tri; ++tri)//triangle
        {
            for (size_t id = 0; id < 3; ++id) // index sommet
            {
                unsigned int current_id = m.triangle_indicies[tri].vertices[id];
                std::cout<< "   Current sommet = "<< current_id << std::endl;
                indices_array.push_back(current_id);
                glm::vec3 current_vertex = glm::vec3(m.vertices[current_id][0], m.vertices[current_id][1], m.vertices[current_id][2]);
                if (pushBackIfNotExists(vertexChecklist, current_vertex))
                {
                    //vertices_array.push_back(m.vertices[current_id][0]);
                    //vertices_array.push_back(m.vertices[current_id][1]);
                    //vertices_array.push_back(m.vertices[current_id][2]);
                    element_pushed+=3;
                }


            }
        }

        //std::cout<<"Elements succesfully pushed -> "<< element_pushed <<std::endl;
        convertVec3ToFloat(m.vertices,vertices_array);
        splitMesh_array.push_back(m.vertices.size());

    }
    std::cout<<" Size of index array : "<< indices_array.size()<<std::endl;
    std::cout<<" Size of vertex array : "<< vertices_array.size() << std::endl;
    std::cout<<" Size of splitMesh array : "<< splitMesh_array.size() <<std::endl;
    std::cout<<" Element of splitMesh array : "<< splitMesh_array[0] <<std::endl;
}

template <typename T>
void printVector(const std::vector<T>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
}

void initializeBuffers() {

    extractSceneData();
    printVector(vertices_array);
    printVector(indices_array);
    std::cout << "[";
    for (size_t i = 0; i < scene_meshes[0]->vertices.size(); ++i) {
        std::cout << scene_meshes[0]->vertices[i][0] <<"-"<< scene_meshes[0]->vertices[i][1] <<"-"<<scene_meshes[0]->vertices[i][2];
        if (i < scene_meshes[0]->vertices.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
    updateMaterialBuffer();
    updateskyColorBuffer();
    vertexBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * vertices_array.size(),vertices_array.data());
    indexBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * indices_array.size(), indices_array.data());
    splitMeshBuffer= cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * splitMesh_array.size(), splitMesh_array.data());
    splitMeshTriBuffer= cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * splitMeshTri_array.size(), splitMeshTri_array.data());


}

void load(int max_x, int max_y,cl::CommandQueue &queue, cl::Program &program, const cl::Device &device)
{

    //Camera setup
    cl_float* cameraData = mainCamera.getCameraData();
    cameraBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * 9, cameraData);
    queue.enqueueWriteBuffer(cameraBuffer, CL_TRUE, 0, sizeof(cl_float) * 9, cameraData);
    cl::NDRange global(max_x, max_y);

    //SkyBox setup
    updateskyColorBuffer();

    //Range and window
    cl::NDRange local(8, 8);
    cl::Kernel load_kernel(program, "loading");

    //Setting up arguments for loading
    load_kernel.setArg(0,max_x);
    load_kernel.setArg(1,max_y);
    load_kernel.setArg(2,cameraBuffer);
    load_kernel.setArg(3,skyColorBuffer);

    //Executing loading in kernel.cl
    queue.enqueueNDRangeKernel(load_kernel, cl::NullRange, global, local);
    cl_int kernelError = queue.finish();
    if (kernelError != CL_SUCCESS) {
        std::cerr << "OpenCL kernel execution error: " << kernelError << std::endl;
        exit(1);
    }
}


void updateCL_Camera(int max_x, int max_y,cl::CommandQueue &queue, cl::Program &program, const cl::Device &device)
{
    //Camera setup
    cl_float* cameraData = mainCamera.getCameraData();
    cameraBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * 9, cameraData);
    queue.enqueueWriteBuffer(cameraBuffer, CL_TRUE, 0, sizeof(cl_float) * 9, cameraData);
    cl::NDRange global(max_x, max_y);

    //Range and window
    cl::NDRange local(8, 8);
    cl::Kernel updateCamera_kernel(program, "updateCamera");

    //Setting up arguments for loading
    updateCamera_kernel.setArg(0,max_x);
    updateCamera_kernel.setArg(1,max_y);
    updateCamera_kernel.setArg(2,cameraBuffer);

    //Executing loading in kernel.cl
    queue.enqueueNDRangeKernel(updateCamera_kernel, cl::NullRange, global, local);
    cl_int kernelError = queue.finish();
    if (kernelError != CL_SUCCESS) {
        std::cerr << "OpenCL kernel execution error: " << kernelError << std::endl;
        exit(1);
    }
}

void updateCL_SkyColor(int max_x, int max_y,cl::CommandQueue &queue, cl::Program &program, const cl::Device &device)
{

    //SkyBox setup
    updateskyColorBuffer();

    //Range and window
    cl::NDRange global(max_x, max_y);
    cl::NDRange local(8, 8);
    cl::Kernel SkyColor_kernel(program, "updateSkyColor");

    //Setting up arguments for loading
    SkyColor_kernel.setArg(0,max_x);
    SkyColor_kernel.setArg(1,max_y);
    SkyColor_kernel.setArg(2,skyColorBuffer);

    //Executing loading in kernel.cl
    queue.enqueueNDRangeKernel(SkyColor_kernel, cl::NullRange, global, local);
    cl_int kernelError = queue.finish();
    if (kernelError != CL_SUCCESS) {
        std::cerr << "OpenCL kernel execution error: " << kernelError << std::endl;
        exit(1);
    }
}



//Fct qui appelle un programme de rendu en gérant les threads pour un device donné
void render(cl::Buffer &buffer, int max_x, int max_y, cl::CommandQueue &queue, cl::Program &program, const cl::Device &device) {


    cl::Kernel kernel(program, "render");
    kernel.setArg(0, buffer);
    kernel.setArg(1, max_x);
    kernel.setArg(2, max_y);
    kernel.setArg(3, vertexBuffer);  // Ajouter le buffer de vertices
    kernel.setArg(4, indexBuffer);   // Ajouter le buffer d'indices
    kernel.setArg(5, meshNbr);   // Le nombre de triangle contenu dans la scene
    kernel.setArg(6,splitMeshBuffer); // Ajouter le buffer
    kernel.setArg(7,splitMeshTriBuffer); // Ajouter le buffer
    kernel.setArg(8,materialsBuffer);
    kernel.setArg(9,skyColorBuffer);

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

#endif //OBJECT_VIEWER_CLFUNCTIONS_H
