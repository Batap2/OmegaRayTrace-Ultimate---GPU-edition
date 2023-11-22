cmake_minimum_required (VERSION 3.16)

project (Object-Viewer)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -std=c++17")
set (source_dir "${PROJECT_SOURCE_DIR}/src/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(src)

file (GLOB source_files "${source_dir}/*.cpp" "${source_dir}/deps/*.cpp" "${source_dir}/deps/*/*.cpp")


add_executable (viewer ${source_files}
        src/Light.h
        src/Material.h
        src/MeshLoader.h
        src/ShaderUtils.h
        src/SceneOperations.h
        src/Object3D.h
        src/Plane.h
        src/Triangle.h
        src/Sphere.h
        src/Square.h)

target_link_libraries(viewer glfw)
target_link_libraries(viewer OpenGL::GL)
target_link_libraries(viewer GLEW)