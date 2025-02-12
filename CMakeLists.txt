cmake_minimum_required(VERSION 3.10.0)
project(game VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# set(CMAKE_SYSTEM_NAME Windows)
# set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(INCLUDE_ROOT "${CMAKE_SOURCE_DIR}/include")
set(SRC_ROOT "${CMAKE_SOURCE_DIR}/src")
set(LIB_ROOT "${CMAKE_SOURCE_DIR}/lib")
set(RUN_ROOT "${CMAKE_SOURCE_DIR}/run")

set(PYTHON_SCRIPT ${CMAKE_SOURCE_DIR}/generate_path_compare.py)

add_custom_target(
    ShaderCompile
    COMMAND cmd /c ${CMAKE_SOURCE_DIR}/shader.bat
    COMMENT "Running shader.bat before building target"
)

add_custom_target(
    MoveLib
    COMMAND cmd /c ${CMAKE_SOURCE_DIR}/move_lib.lnk
    COMMENT "Running move_lib.lnk before building target"
)

add_subdirectory(src/content_manager) # 确保 ContentManager target 被定义

add_custom_command(
    OUTPUT "${RUN_ROOT}/Path"
    COMMAND $<TARGET_FILE:ContentManager> # 使用 TARGET_FILE 生成器表达式, 修正路径问题
    COMMENT "Generate PATH"
    VERBATIM
)
add_custom_target(GeneratePath
    DEPENDS "${RUN_ROOT}/Path"
)

add_custom_command(
    OUTPUT ${SRC_ROOT}/file/generated_function.c ${INCLUDE_ROOT}/file/generated_function.h
    COMMAND python.exe ${PYTHON_SCRIPT}
    COMMENT "Running Python script to generate code..."
    VERBATIM
)
add_custom_target(GenerateCode
    DEPENDS ${SRC_ROOT}/file/generated_function.c ${INCLUDE_ROOT}/file/generated_function.h
)


add_executable(game ${SRC_ROOT}/main.c
                    ${SRC_ROOT}/sqlite3_alloc_func.c
                    ${SRC_ROOT}/content_manager/content_manager.c
                    ${SRC_ROOT}/texture_generate/textureG.c
                    ${SRC_ROOT}/G_game.c
                    ${SRC_ROOT}/G_file/G_file.c
                    ${SRC_ROOT}/G_file/path_compare.c
                    ${SRC_ROOT}/G_graphic.c
                    ${SRC_ROOT}/version.rc
                    ${SRC_ROOT}/G_log.c
                    ${SRC_ROOT}/G_music.c
                    ${SRC_ROOT}/G_world.c
                    ${SRC_ROOT}/G_list.c
                    ${SRC_ROOT}/G_stack.c
                    ${SRC_ROOT}/G_timer.c
                    ${SRC_ROOT}/G_text.c
                    ${SRC_ROOT}/G_pop_window.c
                    ${SRC_ROOT}/G_scene.c
                    ${SRC_ROOT}/G_resource.c
                    ${SRC_ROOT}/SDL_complement.c
                    ${SRC_ROOT}/spirv_reflect/spirv_reflect.c
                    ${SRC_ROOT}/spirv_reflect/shader_resolve.c
                    ${SRC_ROOT}/vk_code/vk_judge.c
                    ${SRC_ROOT}/vk_code/vk_instace.c
                    ${SRC_ROOT}/vk_code/vk_surface.c
                    ${SRC_ROOT}/vk_code/vk_device.c
                    ${SRC_ROOT}/vk_code/vk_alloc_func.c
                    ${SRC_ROOT}/vk_code/vk_queue.c
                    ${SRC_ROOT}/vk_code/vk_swapchain.c
                    ${SRC_ROOT}/vk_code/vk_commandPool.c
                    ${SRC_ROOT}/vk_code/vk_version.c
                    ${SRC_ROOT}/vk_code/vk_buffer.c
                    ${SRC_ROOT}/vk_code/vk_computeShader.c
                    ${SRC_ROOT}/vk_code/vk_depth.c
                    ${SRC_ROOT}/vk_code/vk_descriptorPool.c
                    ${SRC_ROOT}/vk_code/vk_framebuffer.c
                    ${SRC_ROOT}/vk_code/vk_pipeline.c
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
                            # "${INCLUDE_ROOT}/GLFW"
                            # "${INCLUDE_ROOT}/libpng"
                            "${INCLUDE_ROOT}/SDL3"
                            "${INCLUDE_ROOT}/SDL3_mixer"
                            "${INCLUDE_ROOT}/SDL3_image"
                            "${INCLUDE_ROOT}/vulkan"
                            "${INCLUDE_ROOT}/vk_video"
                            "${INCLUDE_ROOT}/spirv_reflect"
                            "${INCLUDE_ROOT}/zlib"
                            "${INCLUDE_ROOT}/textureG"
                            "${INCLUDE_ROOT}/G_file"
                            "${INCLUDE_ROOT}/contentManager")

target_link_libraries(game PRIVATE
                    sqlite3.a
                    SDL3.a
                    SDL3_mixer.a
                    SDL3_image.a
                    box2d.a
                    unistring.a
                    "${LIB_ROOT}/vulkan/vulkan-1.lib"
                    freetyped.a
                    bz2.a z.a setupapi.a winmm.a imm32.a version.a)

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

target_compile_options(game PUBLIC -O0 -Wall)
# target_compile_options(game PUBLIC -O0 -Wall -Wextra)
# target_compile_options(game PUBLIC -Os -Wall)

#  修正后的依赖关系:
add_dependencies(GeneratePath ShaderCompile MoveLib) # GeneratePath 依赖 ShaderCompile 和 MoveLib (先执行)
add_dependencies(GeneratePath ContentManager)   # GeneratePath 依赖 ContentManager (在 ShaderCompile, MoveLib 之后执行)
add_dependencies(GenerateCode GeneratePath)    # GenerateCode 依赖 GeneratePath
add_dependencies(game GenerateCode)         # game 依赖 GenerateCode

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run)