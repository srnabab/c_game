cmake_minimum_required(VERSION 3.5.0)
project(game VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include")
set(SRC_ROOT "${CMAKE_SOURCE_DIR}/src")
set(LIB_ROOT "${CMAKE_SOURCE_DIR}/lib")
set(RUN_ROOT "${CMAKE_SOURCE_DIR}/run")

set(PYTHON_SCRIPT ${CMAKE_SOURCE_DIR}/generate_path_compare.py)

add_custom_command(
    OUTPUT ${SRC_ROOT}file/generated_function.c ${INCLUDE_ROOT}/file/generated_function.h
    COMMAND python.exe ${PYTHON_SCRIPT}
    COMMENT "Running Python script to generate code..."
    VERBATIM
)
add_custom_target(GenerateCode DEPENDS ${SRC_ROOT}file/generated_function.c ${INCLUDE_ROOT}/file/generated_function.h)

add_subdirectory(src/texture_generate)
add_subdirectory(src/vma)

message(STATUS "Include directories for MyExecutable: ${INCLUDE_ROOT}")
message(STATUS "Src directories for MyExecutable: ${SRC_ROOT}")

add_custom_target(
    ShaderCompile
    COMMAND cmd /c ${CMAKE_SOURCE_DIR}/shader.bat
    COMMENT "Running script.py before building target"
)

add_custom_target(
    MoveLib
    COMMAND cmd /c ${CMAKE_SOURCE_DIR}/move_lib.lnk
    COMMENT "Running script.py before building target"
)

add_executable(game ${SRC_ROOT}/main.c 
                    ${SRC_ROOT}/game.c
                    ${SRC_ROOT}/file/file.c 
                    ${SRC_ROOT}/file/path_compare.c
                    ${SRC_ROOT}/graphic.c
                    ${SRC_ROOT}/judge.c
                    ${SRC_ROOT}/version.rc
                    ${SRC_ROOT}/log.c
                    ${SRC_ROOT}/music.c
                    ${SRC_ROOT}/world.c
                    ${SRC_ROOT}/stack.c
                    ${SRC_ROOT}/timer.c
                    ${SRC_ROOT}/text.c
                    ${SRC_ROOT}/pop_window.c
                    ${SRC_ROOT}/SDL_complement.c
                    ${SRC_ROOT}/vk_code/vk_instace.c 
                    ${SRC_ROOT}/vk_code/vk_surface.c
                    ${SRC_ROOT}/vk_code/vk_device.c 
                    ${SRC_ROOT}/vk_code/vk_queue.c
                    ${SRC_ROOT}/vk_code/vk_swapchain.c 
                    ${SRC_ROOT}/vk_code/vk_commandPool.c
                    ${SRC_ROOT}/vk_code/vk_version.c 
                    ${SRC_ROOT}/vk_code/vk_buffer.c
                    ${SRC_ROOT}/vk_code/vk_computeShader.c 
                    ${SRC_ROOT}/vk_code/vk_depth.c
                    ${SRC_ROOT}/vk_code/vk_descriptorPool.c 
                    ${SRC_ROOT}/vk_code/vk_framebuffer.c
                    ${SRC_ROOT}/vk_code/vk_graphicsPipeline.c 
                    ${SRC_ROOT}/vk_code/vk_image.c
                    ${SRC_ROOT}/vk_code/vk_index.c 
                    ${SRC_ROOT}/vk_code/vk_move.c
                    ${SRC_ROOT}/vk_code/vk_vertex.c
                    ${SRC_ROOT}/vk_code/vk_uniform.c
                    ${SRC_ROOT}/vk_code/vk_texture.c
                    ${SRC_ROOT}/vk_code/vk_synchronize.c
                    ${SRC_ROOT}/vk_code/vk_shader.c
                    ${SRC_ROOT}/vk_code/vk_recreate.c
                    ${SRC_ROOT}/vk_code/vk_present.c)

target_include_directories(game PUBLIC ${INCLUDE_ROOT}
                            "${INCLUDE_ROOT}/cglm"
                            "${INCLUDE_ROOT}/vk_code_h"
                            "${INCLUDE_ROOT}/GLFW"
                            # "${INCLUDE_ROOT}/libpng"
                            "${INCLUDE_ROOT}/SDL3"
                            "${INCLUDE_ROOT}/SDL3_mixer"
                            "${INCLUDE_ROOT}/SDL3_image"
                            "${INCLUDE_ROOT}/vulkan"
                            "${INCLUDE_ROOT}/vk_video"
                            "${INCLUDE_ROOT}/zlib"
                            "${INCLUDE_ROOT}/textureG"
                            "${INCLUDE_ROOT}/file")

target_link_libraries(game z SDL3 SDL3_mixer SDL3_image box2d unistring "C:/D/game_c_template_aw/lib/vulkan/vulkan-1.lib" textureG VMA)

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

target_compile_options(game PUBLIC -O0 -Wall)
# target_compile_options(game PUBLIC -O0 -Wall -Wextra)
# target_compile_options(game PUBLIC -O2 -Wall)

add_dependencies(${PROJECT_NAME} GenerateCode ShaderCompile MoveLib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run)