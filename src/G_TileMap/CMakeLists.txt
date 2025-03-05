cmake_minimum_required(VERSION 3.10.0)
project(tileSet VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_executable(tileSet G_TileSetGenerate.c)

target_include_directories(tileSet PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(tileSet PUBLIC SDL3.a SDL3_image.a
                        bz2.a z.a setupapi.a winmm.a imm32.a version.a)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run)