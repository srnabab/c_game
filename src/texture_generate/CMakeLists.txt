add_library(textureG SHARED textureG.c)

message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(textureG PRIVATE "${INCLUDE_ROOT}")

target_link_libraries(textureG PRIVATE freetype SDL3)
target_compile_options(textureG PRIVATE -O0 -Wall)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/textureG)