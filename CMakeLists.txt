cmake_minimum_required(VERSION 3.20)
project(opencl-hello)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS  "-g3")

file(COPY ./src/hello_triangle.cl DESTINATION ./${BINARY_DIR})

find_package(OpenCL REQUIRED)

add_executable(${PROJECT_NAME} ./src/main.cpp)
target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL)
