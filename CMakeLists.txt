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
set(IMPLOT_DIR ${CMAKE_SOURCE_DIR}/deps/ImPlot)
set(SOURCE_IMPLOT
${CMAKE_SOURCE_DIR}/deps/ImPlot/implot.cpp
${CMAKE_SOURCE_DIR}/deps/ImPlot/implot_items.cpp
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
${CMAKE_SOURCE_DIR}/src/Graphics/PostProcessManager.cpp
${CMAKE_SOURCE_DIR}/src/Graphics/FrameBuffer.cpp
${CMAKE_SOURCE_DIR}/src/Graphics/ShadowMap.cpp
${CMAKE_SOURCE_DIR}/src/Graphics/Skybox.cpp
${CMAKE_SOURCE_DIR}/src/Graphics/ParticleManager.cpp
${CMAKE_SOURCE_DIR}/src/Resources/ResourceManager.cpp
${CMAKE_SOURCE_DIR}/src/Scenes/SceneManager.cpp
${CMAKE_SOURCE_DIR}/src/Scenes/Scene.cpp
${CMAKE_SOURCE_DIR}/src/Scenes/GameObject.cpp
${CMAKE_SOURCE_DIR}/src/Development/Editor.cpp
${CMAKE_SOURCE_DIR}/src/Development/EditorTools.cpp

${CMAKE_SOURCE_DIR}/src/Development/Terminal.cpp
${CMAKE_SOURCE_DIR}/src/Development/EditorGrid.cpp
${CMAKE_SOURCE_DIR}/src/Development/Gizmo.cpp

${CMAKE_SOURCE_DIR}/src/Utils/Telemetry.cpp
${CMAKE_SOURCE_DIR}/src/Utils/MemoryTracker.cpp
${CMAKE_SOURCE_DIR}/src/Utils/Math.h
${CMAKE_SOURCE_DIR}/src/Audio/AudioManager.cpp
${CMAKE_SOURCE_DIR}/src/Components/PlayerController.cpp

${CMAKE_SOURCE_DIR}/src/Development/Gui/EditorUI.cpp
${CMAKE_SOURCE_DIR}/src/Development/Gui/ToolBar.cpp
${CMAKE_SOURCE_DIR}/src/Development/Gui/InspectorPanel.cpp
${CMAKE_SOURCE_DIR}/src/Development/Gui/HierarchyPanel.cpp
)

add_executable(KleeneRuntime ${SOURCE_ENGINE} ${SOURCE_IMGUI} ${SOURCE_IMPLOT})

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
${CMAKE_SOURCE_DIR}/deps/Nvidia
${CMAKE_SOURCE_DIR}/deps/ImGui
${CMAKE_SOURCE_DIR}/deps/ImGui/backends
${CMAKE_SOURCE_DIR}/deps/SDL2/include
${CMAKE_SOURCE_DIR}/deps/ImPlot
${CMAKE_SOURCE_DIR}/deps/Miniaudio
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

