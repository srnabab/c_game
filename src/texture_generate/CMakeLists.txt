add_library(textureG SHARED textureG.c)

message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(textureG PRIVATE "${INCLUDE_ROOT}")

target_link_libraries(textureG PRIVATE freetype SDL3)
target_compile_options(textureG PRIVATE -O0 -Wall)

add_executable(imageGenerate textureG.c main.c)

target_include_directories(imageGenerate PRIVATE "${INCLUDE_ROOT}")

target_link_libraries(imageGenerate PRIVATE freetype SDL3)
target_compile_options(imageGenerate PRIVATE -O0 -Wall)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/textureG)