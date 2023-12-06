cmake_minimum_required(VERSION 3.26)
project(Progetto)

set(CMAKE_CXX_STANDARD 14)

add_executable(Progetto
        c++/main.cpp)

find_package(unofficial-restbed CONFIG REQUIRED)
target_link_libraries(Progetto PRIVATE unofficial::restbed::restbed)