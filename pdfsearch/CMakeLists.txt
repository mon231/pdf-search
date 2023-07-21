set(CMAKE_CXX_STANDARD 20)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE HEADER_FILES "*.hpp" "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp" "*.c" "*.asm" "*.s")

add_executable(pdfsearch ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(pdfsearch PRIVATE opencv_core)
target_include_directories(pdfsearch PUBLIC "${CMAKE_SOURCE_DIR}/opencv/include")
