cmake_minimum_required(VERSION 3.5.0)
project(game VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

add_executable(game src/main.c 
                    src/cJSON.c
                    src/game.c
                    src/file.c 
                    src/flow.c 
                    src/judge.c
                    src/version.rc
                    src/vk_code/vk_instace.c 
                    src/vk_code/vk_surface.c
                    src/vk_code/vk_physical_device.c 
                    src/vk_code/vk_queue.c
                    src/vk_code/vk_swapchain.c 
                    src/vk_code/vk_commandPool.c
                    src/vk_code/vk_version.c 
                    src/vk_code/vk_buffer.c
                    src/vk_code/vk_computeShader.c 
                    src/vk_code/vk_depth.c
                    src/vk_code/vk_descriptorPool.c 
                    src/vk_code/vk_framebuffer.c
                    src/vk_code/vk_graphicsPipeline.c 
                    src/vk_code/vk_image.c
                    src/vk_code/vk_index.c 
                    src/vk_code/vk_move.c
                    src/vk_code/vk_vertex.c
                    src/vk_code/vk_uniform.c
                    src/vk_code/vk_texture.c
                    src/vk_code/vk_synchronize.c
                    src/vk_code/vk_shader.c
                    src/vk_code/vk_recreate.c
                    src/vk_code/vk_present.c)

target_include_directories(game PUBLIC "include" 
                            "include/cJSON"
                            "include/cglm"
                            "include/vk_code_h"
                            "include/GLFW"
                            "include/libpng"
                            "include/SDL3"
                            "include/vulkan"
                            "include/vk_video"
                            "include/zlib")

target_link_libraries(game z SDL3 libpng glfw3 "C:/D/c_game/lib/vulkan/vulkan-1.lib")

#4set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run)