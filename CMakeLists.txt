set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.5)

project(permTree)

add_executable(ptree main.cpp)

include_directories(
    thirdparty/Utils
)

target_link_libraries(ptree pthread)