add_executable(imageGenerate main.c textureG.c)

target_include_directories(imageGenerate PRIVATE "${INCLUDE_ROOT}")

target_link_libraries(imageGenerate PRIVATE
                    freetyped.a bz2 z.a
                    SDL3.a
                    setupapi winmm imm32 version )
target_compile_options(imageGenerate PRIVATE -O2 -Wall)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run)