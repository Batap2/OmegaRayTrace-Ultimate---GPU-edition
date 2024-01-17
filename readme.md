# OmegaRayTrace-Ultimate---GPU-edition

This project is a mesh viewer and real-time path-tracing renderer on GPU using OpenCL 3. It supports all types of files and embedded textures. It is written in C++, with C for the OpenCL part and GLSL for shaders. For the OpenGL rendering part, a physically-based rendering (PBR) approach is implemented.

## Usage

To compile, create a directory called `build` and create Makefile using the cmake build system

    mkdir build && cd build
    cmake ..
    make

Once compiled, launch the viewer and open a file with the "file" button

![](demo/openFile.gif)

## Input :
Translate camera : a z e q s d
Rotate camera : right clic + mouse
Change camera speed : scrollwheel

## Render Mode 
Fast : OpenGL rendering pipeline
RayTraced : OpenCL on GPU, path tracing rendering

![](demo/raytrace.gif)

## Materials :

Materials can be modified by clicking on an object in the scene object list.

## Lights :

Lights can be added or added by clicking on a light in the scene lights list. There are only used for the Fast render mode

## Dependencies and Fork

This project is forked from https://github.com/amanshenoy/mesh-viewer.
It using GLFW for the window, ImGui for the interface, OpenCL for the path tracing render on GPU, Assimp, dirent & imgui-filebrowser for importing 3D files and glm.
