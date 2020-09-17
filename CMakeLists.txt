set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.5)

project(permTree)

add_executable(ptree main.cpp)

include_directories(
    thirdparty/Utils
)