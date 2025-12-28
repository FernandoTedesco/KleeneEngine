cmake_minimum_required(VERSION 3.14)
project("KleeneEngine")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)



set(SDL2_LIB "${CMAKE_SOURCE_DIR}/deps/SDL2/lib/x64/SDL2.lib")
set(SDL2_MAIN_LIB "${CMAKE_SOURCE_DIR}/deps/SDL2/lib/x64/SDL2main.lib")
set(SDL2_DLL "${CMAKE_SOURCE_DIR}/deps/SDL2/lib/x64/SDL2.dll")
set(SOURCE_IMGUI
${CMAKE_SOURCE_DIR}/deps/ImGui/imgui.cpp
${CMAKE_SOURCE_DIR}/deps/ImGui/imgui_draw.cpp
${CMAKE_SOURCE_DIR}/deps/ImGui/imgui_tables.cpp
${CMAKE_SOURCE_DIR}/deps/ImGui/imgui_widgets.cpp
${CMAKE_SOURCE_DIR}/deps/ImGui/backends/imgui_impl_sdl2.cpp
${CMAKE_SOURCE_DIR}/deps/ImGui/backends/imgui_impl_opengl3.cpp
)

set(SOURCE_ENGINE
${CMAKE_SOURCE_DIR}/src/main.cpp
${CMAKE_SOURCE_DIR}/src/glad.c
${CMAKE_SOURCE_DIR}/src/Core/Window.cpp
${CMAKE_SOURCE_DIR}/src/Core/Engine.cpp
${CMAKE_SOURCE_DIR}/src/Core/Camera.cpp
${CMAKE_SOURCE_DIR}/src/Core/Input.cpp
${CMAKE_SOURCE_DIR}/src/Graphics/Shader.cpp
${CMAKE_SOURCE_DIR}/src/Graphics/Mesh.cpp
${CMAKE_SOURCE_DIR}/src/Graphics/Texture.cpp
${CMAKE_SOURCE_DIR}/src/Graphics/Renderer.cpp
${CMAKE_SOURCE_DIR}/src/Graphics/Material.cpp
${CMAKE_SOURCE_DIR}/src/Resources/ResourceManager.cpp
${CMAKE_SOURCE_DIR}/src/Scenes/SceneManager.cpp
${CMAKE_SOURCE_DIR}/src/Scenes/GameObject.cpp
${CMAKE_SOURCE_DIR}/src/Development/Editor.cpp
${CMAKE_SOURCE_DIR}/src/Development/Terminal.cpp
${CMAKE_SOURCE_DIR}/src/Development/EditorGrid.cpp
${CMAKE_SOURCE_DIR}/src/Development/SystemMetrics.cpp
)

add_executable(KleeneRuntime ${SOURCE_ENGINE} ${SOURCE_IMGUI})

find_program(CLANG_FORMAT_EXE clang-format)
if(CLANG_FORMAT_EXE)
file(GLOB_RECURSE FORMAT_FILES
${CMAKE_SOURCE_DIR}/src/*.cpp
${CMAKE_SOURCE_DIR}/src/*.h
${CMAKE_SOURCE_DIR}/src/*.hpp)
add_custom_target(FORMAT_CODE COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${FORMAT_FILES})
add_dependencies(KleeneRuntime FORMAT_CODE)
endif()



target_compile_definitions(KleeneRuntime PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

target_include_directories(KleeneRuntime PRIVATE
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/deps
${CMAKE_SOURCE_DIR}/deps/ImGui
${CMAKE_SOURCE_DIR}/deps/ImGui/backends
${CMAKE_SOURCE_DIR}/deps/SDL2/include
)

target_link_libraries(KleeneRuntime PRIVATE
${SDL2_MAIN_LIB}
${SDL2_LIB}
shell32
)

add_custom_command(TARGET KleeneRuntime POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different
"${SDL2_DLL}"
$<TARGET_FILE_DIR:KleeneRuntime>
)

