add_executable(ContentManager content_manager.c
                            ${SRC_ROOT}/sqlite3_alloc_func.c)

message(STATUS "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}" )

target_include_directories(ContentManager PRIVATE "${INCLUDE_ROOT}" 
                                            "${INCLUDE_ROOT}/contentManager"
                                            "${INCLUDE_ROOT}/SDL3"
                                            "${INCLUDE_ROOT}/sqlite3")

target_link_libraries(ContentManager PRIVATE SDL3 sqlite3)
target_compile_options(ContentManager PUBLIC -O0 -Wall)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/run)