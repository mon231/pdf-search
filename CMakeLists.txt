project(pdfsearch)

add_subdirectory(opencv)
add_subdirectory(libharu)

add_executable(pdfsearch "pdfsearch/main.cpp")
