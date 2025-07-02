add_executable(ContentManager UUID.c blake_hash.c imageLoadParameter.c contentPath.c aliasNamePair.c deletedRow.c content_manager.c main.c
                            ${SRC_ROOT}/G_file/G_readFile.c
                            ${SRC_ROOT}/sqlite3_alloc_func.c
                            # ${SRC_ROOT}/sqlite3/sqlite3.c
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
                                    blake3.a
                                    Rpcrt4
                                    setupapi winmm imm32 version)
# target_compile_options(ContentManager PUBLIC -O2 -Wall)
target_compile_options(ContentManager PUBLIC -static-libgcc -gdwarf-2 -g -DDEBUG -O0)
# target_compile_options(ContentManager PUBLIC -static-libgcc -gdwarf-2 -g -DDEBUG -O0 -Wall -Wpedantic -Wconversion -Wsign-conversion -Wfloat-conversion -Wdouble-promotion
# target_compile_options(ContentManager PUBLIC -static-libgcc -gdwarf-2 -g -DDEBUG -O0 -Wall -Werror -Wpedantic -Wconversion -Wsign-conversion -Wfloat-conversion -Wdouble-promotion
#  -Wcast-qual -Wnull-dereference -Winit-self -Wmaybe-uninitialized -Wstrict-aliasing=2 -Wshadow -Wformat=2 -Wundef -Wmissing-declarations -Wmissing-prototypes
#  -Wredundant-decls -Wpacked-bitfield-compat -Wuseless-cast -Wlogical-op -Warray-bounds=2 -Wstack-usage=8192 -Winline -std=c17)

set(EXECUTABLE_OUTPUT_PATH ${RUN_ROOT})
