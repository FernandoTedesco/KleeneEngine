cmake_minimum_required(VERSION 3.14)
project("KleeneEngine")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(IMGUI ${CMAKE_SOURCE_DIR}/deps/ImGui/imgui_draw.cpp ${CMAKE_SOURCE_DIR}/deps/ImGui/imgui_tables.cpp 
${CMAKE_SOURCE_DIR}/deps/ImGui/imgui.cpp ${CMAKE_SOURCE_DIR}/deps/ImGui/imgui_widgets.cpp 
${CMAKE_SOURCE_DIR}/deps/ImGui/backends/imgui_impl_sdl2.cpp ${CMAKE_SOURCE_DIR}/deps/ImGui/backends/imgui_impl_opengl3.cpp)

set(KLEENE ${CMAKE_SOURCE_DIR}/src/Graphics/Shader.cpp ${CMAKE_SOURCE_DIR}/src/Graphics/Mesh.cpp ${CMAKE_SOURCE_DIR}/src/Core/Window.cpp 
${CMAKE_SOURCE_DIR}/src/Resources/ResourceManager.cpp ${CMAKE_SOURCE_DIR}/src/Core/Camera.cpp ${CMAKE_SOURCE_DIR}/src/Core/Input.cpp 
${CMAKE_SOURCE_DIR}/src/Graphics/Texture.cpp ${CMAKE_SOURCE_DIR}/src/main.cpp ${CMAKE_SOURCE_DIR}/src/Core/Engine.cpp 
${CMAKE_SOURCE_DIR}/src/Scenes/SceneLoader.cpp ${CMAKE_SOURCE_DIR}/src/Graphics/Renderer.cpp)

add_definitions(-DIMGUI_INPL_OPENGL_LOADER_GLAD)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/deps)
include_directories(${CMAKE_SOURCE_DIR}/deps/ImGui)
include_directories(${CMAKE_SOURCE_DIR}/deps/ImGui/backends)
include_directories(${CMAKE_SOURCE_DIR}/deps/SDL2/include)


link_directories(${CMAKE_SOURCE_DIR}/deps/SDL2/lib/x64)

add_executable(KleeneRuntime  src/glad.c  ${IMGUI} ${KLEENE})

target_link_libraries(KleeneRuntime PRIVATE SDL2 SDL2main shell32) 
