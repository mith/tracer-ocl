cmake_minimum_required(VERSION 3.2)
project (tracer-ocl)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS iostreams)
pkg_search_module(GLFW REQUIRED glfw3)

add_executable(tracer-ocl src/main.cpp src/Tracer.cpp src/Scene.cpp src/Meshloader.cpp)
set_property(TARGET tracer-ocl PROPERTY CXX_STANDARD 14)
set_property(TARGET tracer-ocl PROPERTY CXX_STANDARD_REQUIRED 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic") 
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/usr/local/lib")
include_directories("./include" 
                    "/Users/simon/tmp/regal/include"
                    "/usr/local/include/"
                    ${OpenCL_INCLUDE_DIR} 
                    ${OPENGL_INCLUDE_DIR} 
                    ${GLFW_INCLUDE_DIR}
                    ${BOOST_INCLUDE_DIR})
target_link_libraries(tracer-ocl ${OpenCL_LIBRARIES} 
                                 ${GLFW_LIBRARIES} 
                                 ${OPENGL_LIBRARIES} 
                                 ${Boost_LIBRARIES})
add_custom_command(TARGET tracer-ocl POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_CURRENT_SOURCE_DIR}/src/kernels
                        $<TARGET_FILE_DIR:tracer-ocl>/kernels)
