cmake_minimum_required (VERSION 3.16)

project (Object-Viewer)

# Définir les drapeaux de compilation
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -std=c++17")

# Définir le répertoire des sources
set (source_dir "${PROJECT_SOURCE_DIR}/src/")

# Définir le répertoire de sortie des exécutables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Rechercher les packages nécessaires
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Ajouter le sous-répertoire src
add_subdirectory(src)

# Ajouter les fichiers source
file (GLOB source_files "${source_dir}/*.cpp" "${source_dir}/*.cu" "${source_dir}/deps/*.cpp" "${source_dir}/deps/*/*.cpp")

# Ajouter la prise en charge de CUDA
find_package(CUDA REQUIRED)
enable_language(CUDA)


# Spécifier l'architecture CUDA (ajustez selon vos besoins)
# Spécifier l'architecture CUDA (ajustez selon vos besoins)
set(CMAKE_CUDA_ARCHITECTURES 52 60 61 75)




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
        src/Square.h)

# Lier les bibliothèques nécessaires
target_link_libraries(viewer glfw)
target_link_libraries(viewer OpenGL::GL)
target_link_libraries(viewer GLEW)
target_link_libraries(viewer assimp)
target_link_libraries(viewer ${CUDA_LIBRARIES})

