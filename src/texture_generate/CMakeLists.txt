add_library(textureG SHARED textureG.c)

target_link_libraries(textureG PUBLIC freetype)

target_include_directories(textureG PUBLIC "${INCLUDE_ROOT}/textureG" "${INCLUDE_ROOT}/SDL3")
target_link_libraries(textureG PUBLIC SDL3)
target_compile_options(textureG PUBLIC -O0 -Wall)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run)