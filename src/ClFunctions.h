//
// Created by mylgasia on 28/12/23.
//

#ifndef OBJECT_VIEWER_CLFUNCTIONS_H
#define OBJECT_VIEWER_CLFUNCTIONS_H

#include "globals.h"
#include <CL/cl.h>
#include <CL/cl2.hpp>
#include "CL/cl_gl.h"

//-----------------------UTILITARIES----------------------------------------//

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

template <typename T>
void printVector(const std::vector<T>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << (int)vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
}


//To Write an image
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
//--------------------------------------------------UPDATE BUFFER-----------------------------------------------------//

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

void updateLightBuffer()
{
    lights_array.clear();
    lightNbr = (int)scene_lights.size();
    for(size_t i = 0; i < lightNbr; i++) {
        std::vector<float> current_light_data = scene_lights[i]->getLightData();
        for(int j = 0; j < 11;j++)
        {
            float current_light_elem = current_light_data[j];
            lights_array.push_back(current_light_elem);
        }
    }
    std::cout<<"Number of elements in lights array : "<< lights_array.size()<<std::endl;
    std::cout<<"Number of lights : "<< lights_array.size() / 11 <<std::endl;
    lightsBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * lights_array.size(),lights_array.data());

}


void updateTextureBuffer()
{
    texturesData_array.clear();
    texturesId_array.clear();
    textureNbr = 0;
    std::vector<std::vector<unsigned char>> dataValid;
    std::vector<int> offset_array;
    offset_array.push_back(0);
    //For all meshes
    for (size_t i = 0; i < scene_meshes.size(); i++) {

        Mesh * m = scene_meshes[i];
        int tex_id = -1;
        //If it has a texture to use
        if ((bool)m->material.useTexture) {
            unsigned char w = m->material.diffuse_texture.width;
            unsigned char h = m->material.diffuse_texture.height;
            std::vector<unsigned char> data = m->material.diffuse_texture.data;
            bool find = false;
            //We search if we already have this texture to dodge double same texture
            for (int j = 0; j < dataValid.size(); j++)
            {
                if(dataValid[j] == data) // If we found this text
                {
                    find = true;
                    tex_id = j; //Set tex_id to the texture
                    texturesId_array.push_back(tex_id);
                    break;
                }
            }
            if(!find)
            {
                textureNbr++;
                tex_id = dataValid.size(); // last indice to add a new texture on it and to save this id
                texturesId_array.push_back(tex_id);
                texturesData_array.push_back(w);
                texturesData_array.push_back(h);
                texturesData_array.insert(texturesData_array.end(), data.begin(),data.end());
                dataValid.push_back(data); // new texture added
                offset_array.push_back((data.size()+2)+offset_array[offset_array.size()-1]);
                texturesSize = offset_array[offset_array.size()-1];
            }
        } else {
            texturesId_array.push_back(tex_id);
        }
        }

        for (int i = 0; i < texturesId_array.size() ;i++)
        {
            if(texturesId_array[i] != -1)
            {
                texturesId_array[i] = (int)offset_array[texturesId_array[i]];
            }
        }

    //printVector(offset_array);
    //printVector(texturesId_array);
    //printVector(texturesData_array);
    //std::cout << "Number of elements in texturesData array: " << texturesData_array.size()<<" " << texturesData_array[0]<<" " << texturesData_array[1]<<" "  << texturesData_array[2]<< std::endl;
    //std::cout << "Number of elements in texturesId array: " << texturesId_array.size() <<" "<<texturesId_array[0] << std::endl;
    std::cout << "Number of textures saved : " << textureNbr << std::endl;
    texturesDataBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned char) * texturesData_array.size(),texturesData_array.data());
    texturesIdBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * texturesId_array.size(),texturesId_array.data());


}





//-----------------------------------------DATA EXTRACTOR FROM SCENE ---------------------------------------------------//


void extractSceneData() {
    vertices_array.clear();
    indices_array.clear();
    splitMesh_array.clear();
    splitMeshTri_array.clear();
    uv_array.clear();

    size_t mesh_number = scene_meshes.size();
    meshNbr = mesh_number;
    std::vector<glm::vec3> vertexChecklist;
    std::cout << "Nbr of meshes in the scene : " << mesh_number << "\n";
    meshNbr = (int) mesh_number;
    for (size_t i = 0; i < mesh_number; i++) {
        Mesh m = *scene_meshes[i];
        size_t n_tri = m.triangle_indicies.size();
        size_t n_id = m.triangle_indicies.size() * 3;
        size_t n_vertex_float = m.vertices.size() * 3;
        size_t n_uv = m.uv.size();
        splitMeshTri_array.push_back(n_tri);

        std::cout << "Mesh: " << i + 1 << "\n";
        std::cout << " Number of triangles : " << n_tri << std::endl;
        std::cout << " Number of indices : " << n_id << std::endl;
        std::cout << " Number of vertex float : " << n_vertex_float << std::endl;
        std::cout << " Number of uvs float :  " << n_uv << std::endl;
        unsigned int element_pushed = 0;
        for (size_t tri = 0; tri < n_tri; ++tri)//triangle
        {
            for (size_t id = 0; id < 3; ++id) // index sommet
            {
                unsigned int current_id = m.triangle_indicies[tri].vertices[id];
                //std::cout<< "   Current sommet = "<< current_id << std::endl;
                indices_array.push_back(current_id);
                glm::vec3 current_vertex = glm::vec3(m.vertices[current_id][0], m.vertices[current_id][1],
                                                     m.vertices[current_id][2]);

            }

            for (size_t id = 0; id < 3; ++id) // index sommet
            {
                unsigned int current_id = m.triangle_indicies[tri].vertices[id];
                //std::cout<< "   Current sommet = "<< current_id << std::endl;
                indices_array.push_back(current_id);
                glm::vec3 current_vertex = glm::vec3(m.vertices[current_id][0], m.vertices[current_id][1],
                                                     m.vertices[current_id][2]);
                if (pushBackIfNotExists(vertexChecklist, current_vertex)) {
                    //vertices_array.push_back(m.vertices[current_id][0]);
                    //vertices_array.push_back(m.vertices[current_id][1]);
                    //vertices_array.push_back(m.vertices[current_id][2]);
                    element_pushed += 3;
                }
            }

            //std::cout<<"Elements succesfully pushed -> "<< element_pushed <<std::endl;
            convertVec3ToFloat(m.vertices, vertices_array);
            splitMesh_array.push_back(m.vertices.size());

        }
        for (int uv_id = 0; uv_id < m.uv.size();uv_id++)
        {
            uv_array.push_back(m.uv[uv_id]);
        }
        splitUV_array.push_back(n_uv);

        std::cout << " Size of index array : " << indices_array.size() << std::endl;
        std::cout << " Size of vertex array : " << vertices_array.size() << std::endl;
        std::cout << " Size of splitMesh array : " << splitMesh_array.size() << std::endl;
        std::cout << " Element of splitMesh array : " << splitMesh_array[0] << std::endl;
        std::cout << " Element of splitUV array : " << splitUV_array[0] << std::endl;
        std::cout << " Size of uv array : " <<uv_array.size() << std::endl;
        std::cout << "Size of split uv "<< splitUV_array.size() <<std::endl;

    }
}
//---------------------------------------------FIRST INIT OF BUFFERS -------------------------------------------------------//

void initializeBuffers() {

    extractSceneData();

    updateMaterialBuffer();
    updateskyColorBuffer();
    updateLightBuffer();
    updateTextureBuffer();
    vertexBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * vertices_array.size(),vertices_array.data());
    indexBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * indices_array.size(), indices_array.data());
    splitMeshBuffer= cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * splitMesh_array.size(), splitMesh_array.data());
    splitMeshTriBuffer= cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * splitMeshTri_array.size(), splitMeshTri_array.data());
    uvBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * uv_array.size(), uv_array.data());
    splitUVBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(unsigned int) * splitUV_array.size(), splitUV_array.data());
}


//----------------------------------------------- KERNELS ----------------------------------------------------------------//

//LOADING KERNEL - To set up initial scene data
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
    load_kernel.setArg(4, meshNbr);
    load_kernel.setArg(5,materialsBuffer);
    load_kernel.setArg(6, vertexBuffer);  // Ajouter le buffer de vertices
    load_kernel.setArg(7, indexBuffer);   // Ajouter le buffer d'indices
    load_kernel.setArg(8,splitMeshBuffer); // Ajouter le buffer
    load_kernel.setArg(9,splitMeshTriBuffer); // Ajouter le buffer
    load_kernel.setArg(10,lightsBuffer);
    load_kernel.setArg(11,lightNbr);
    load_kernel.setArg(12,texturesDataBuffer);
    load_kernel.setArg(13,texturesIdBuffer);
    load_kernel.setArg(14,texturesSize);
    load_kernel.setArg(15,uvBuffer);
    load_kernel.setArg(16,splitUVBuffer);

    //load_kernel.setArg(12,textures_array[0]);
    //Executing loading in kernel.cl
    queue.enqueueNDRangeKernel(load_kernel, cl::NullRange, global, local);
    cl_int kernelError = queue.finish();
    if (kernelError != CL_SUCCESS) {
        std::cerr << "OpenCL kernel execution error: " << kernelError << std::endl;
        exit(1);
    }
}

//To update the camera on the GPU
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

//To update SkyColor on the GPU
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

//To update Materials on the GPU
void updateCL_Materials(int max_x, int max_y,cl::CommandQueue &queue, cl::Program &program, const cl::Device &device)
{

    updateMaterialBuffer();

    //Range and window
    cl::NDRange global(max_x, max_y);
    cl::NDRange local(8, 8);
    cl::Kernel Materials_kernel(program, "updateMaterials");

    //Setting up arguments for loading
    Materials_kernel.setArg(0,max_x);
    Materials_kernel.setArg(1,max_y);
    Materials_kernel.setArg(2,meshNbr);
    Materials_kernel.setArg(3,materialsBuffer);


    //Executing loading in kernel.cl
    queue.enqueueNDRangeKernel(Materials_kernel, cl::NullRange, global, local);
    cl_int kernelError = queue.finish();
    if (kernelError != CL_SUCCESS) {
        std::cerr << "OpenCL kernel execution error: " << kernelError << std::endl;
        exit(1);
    }
}

//To update Lights on the GPU
void updateCL_Lights(int max_x, int max_y,cl::CommandQueue &queue, cl::Program &program, const cl::Device &device)
{

    updateLightBuffer();
    std::cout<<"Lights NBR " << lightNbr<<std::endl;

    //Range and window
    cl::NDRange global(max_x, max_y);
    cl::NDRange local(8, 8);
    cl::Kernel Lights_kernel(program, "updateLights");

    //Setting up arguments for loading
    Lights_kernel.setArg(0,max_x);
    Lights_kernel.setArg(1,max_y);
    Lights_kernel.setArg(2,lightNbr);
    Lights_kernel.setArg(3,lightsBuffer);


    //Executing loading in kernel.cl
    queue.enqueueNDRangeKernel(Lights_kernel, cl::NullRange, global, local);
    cl_int kernelError = queue.finish();
    if (kernelError != CL_SUCCESS) {
        std::cerr << "OpenCL kernel execution error: " << kernelError << std::endl;
        exit(1);
    }
}



//To render a frame by raytracing on GPU
void render(cl::Buffer &buffer, int max_x, int max_y, cl::CommandQueue &queue, cl::Program &program, const cl::Device &device) {
    cl::Kernel kernel(program, "render");
    kernel.setArg(0, buffer);
    kernel.setArg(1, max_x);
    kernel.setArg(2, max_y);


    cl::NDRange global(max_x, max_y);
    cl::NDRange local(8, 8);

    unsigned int random_int = rand(); // Random int value
    cl::Kernel randInt(program,"getRandomIntInGPU");
    randInt.setArg(0,random_int);
    queue.enqueueNDRangeKernel(randInt, cl::NullRange, global, local);

    queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
    cl_int kernelError = queue.finish();
    if (kernelError != CL_SUCCESS) {
        std::cerr << "OpenCL kernel execution error: " << kernelError << std::endl;
        exit(1);
    }
}

//To setup the OpenCL environnement
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

//To call render on GPU and mesure the rendering time
void renderImage(cl::Buffer& buffer, int nx, int ny, cl::CommandQueue& queue, cl::Program& program, const std::vector<cl::Device>& devices) {
    // Measure the time before rendering
    clock_t start_time = clock();

    // Render
    render(buffer, nx, ny, queue, program, devices[0]);

    // Measure the time after rendering
    clock_t end_time = clock();
    double duration = double(end_time - start_time) / CLOCKS_PER_SEC;

    //std::cerr << "Rendering time: " << duration << " seconds" << std::endl;
}



#endif //OBJECT_VIEWER_CLFUNCTIONS_H
