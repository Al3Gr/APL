cmake_minimum_required(VERSION 3.26)
project(testServer)
set(CMAKE_CXX_STANDARD 17)

add_executable(testServer main.cpp
        prova.cpp
        prova.h)
find_package(unofficial-restbed CONFIG REQUIRED)
target_link_libraries(testServer PRIVATE unofficial::restbed::restbed)

#find_package(nlohmann_json 3.2.0 REQUIRED)