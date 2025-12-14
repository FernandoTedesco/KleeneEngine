cmake_minimum_required(VERSION 3.14)
project("KleeneEngine")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/deps/SDL2/include)

link_directories(${CMAKE_SOURCE_DIR}/deps/SDL2/lib/x64)

add_executable(KleeneRuntime src/main.cpp src/glad.c src/Graphics/Shader.cpp src/Graphics/Mesh.cpp src/Core/Window.cpp src/Resources/ResourceManager.cpp)
target_link_libraries(KleeneRuntime PRIVATE SDL2 SDL2main shell32) 
