add_executable(ContentManager content_manager.c main.c 
                            ${SRC_ROOT}/sqlite3_alloc_func.c
                            ${SRC_ROOT}/G_allocator.c)

message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}" )

target_include_directories(ContentManager PRIVATE "${INCLUDE_ROOT}" 
                                            "${INCLUDE_ROOT}/contentManager"
                                            "${INCLUDE_ROOT}/SDL3"
                                            "${INCLUDE_ROOT}/sqlite3"
                                            "${INCLUDE_ROOT}/G_file"
                                            "${INCLUDE_ROOT}/uthash")

target_link_libraries(ContentManager PRIVATE 
                                    sqlite3.a
                                    SDL3.a
                                    setupapi winmm imm32 version)
# target_compile_options(ContentManager PUBLIC -O2 -Wall)
target_compile_options(ContentManager PUBLIC -static-libgcc -O0 -std=c17)

set(EXECUTABLE_OUTPUT_PATH ${RUN_ROOT})
