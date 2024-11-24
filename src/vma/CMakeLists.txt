add_library(VMA SHARED ${SRC_ROOT}/vma/vma_usage.cpp
                ${INCLUDE_ROOT}/vma/vk_mem_alloc.h)

target_include_directories(VMA PRIVATE ${INCLUDE_ROOT})

target_link_libraries(VMA PRIVATE "${CMAKE_SOURCE_DIR}/lib/vulkan/vulkan-1.lib")

set_target_properties(VMA PROPERTIES CXX_STANDARD 20)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/vma)