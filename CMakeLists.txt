cmake_minimum_required(VERSION 3.2)
project (tracer-ocl)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED COMPONENTS iostreams)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(YAML REQUIRED yaml-cpp)

file(GLOB SOURCES src/*.cpp)

add_executable(tracer-ocl ${SOURCES})
set_property(TARGET tracer-ocl PROPERTY CXX_STANDARD 14)
set_property(TARGET tracer-ocl PROPERTY CXX_STANDARD_REQUIRED 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fcolor-diagnostics") 
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto") 
include_directories("./include" 
                    ${OpenCL_INCLUDE_DIR} 
                    ${OPENGL_INCLUDE_DIR} 
                    ${GLFW_INCLUDE_DIR}
                    ${BOOST_INCLUDE_DIR}
                    ${YAML_INCLUDE_DIR})
target_link_libraries(tracer-ocl ${OpenCL_LIBRARIES} 
                                 ${GLFW_LIBRARIES} 
                                 ${OPENGL_LIBRARIES} 
                                 ${Boost_LIBRARIES}
                                 ${YAML_LIBRARIES})
