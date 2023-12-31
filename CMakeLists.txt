cmake_minimum_required(VERSION 3.16)

project(Object-Viewer)

# Définir les drapeaux de compilation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -std=c++17")

# Définir le répertoire des sources
set(source_dir "${PROJECT_SOURCE_DIR}/src/")

# Définir le répertoire de sortie des exécutables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Rechercher les packages nécessaires
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenCL REQUIRED)

# Ajouter le sous-répertoire src
add_subdirectory(src)

# Ajouter les fichiers source
file(GLOB source_files "${source_dir}/*.cpp" "${source_dir}/deps/*.cpp" "${source_dir}/deps/*/*.cpp")

# Ajouter l'exécutable avec les fichiers source
add_executable(viewer ${source_files}
    src/Light.h
    src/Material.h
    src/MeshLoader.h
    src/ShaderUtils.h
    src/SceneOperations.h
    src/Object3D.h
    src/Plane.h
    src/Triangle.h
    src/Sphere.h
    src/Square.h
        src/ClFunctions.h)

# Lier les bibliothèques nécessaires
target_link_libraries(viewer glfw)
target_link_libraries(viewer OpenGL::GL)
target_link_libraries(viewer GLEW)
target_link_libraries(viewer assimp)
target_link_libraries(viewer ${OpenCL_LIBRARIES})

# Inclure les en-têtes OpenCL
target_include_directories(viewer PRIVATE ${OpenCL_INCLUDE_DIRS})

# Définir les drapeaux de compilation pour OpenCL
target_compile_definitions(viewer PRIVATE ${OpenCL_DEFINITIONS})


