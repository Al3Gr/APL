cmake_minimum_required(VERSION 3.26)
project(Progetto)

set(CMAKE_CXX_STANDARD 14)

add_executable(Progetto
        server.cpp
        SocketTCP.h
        exceptions/SocketException.h
        minio-client/MinIOUploader.h
        minio-client/MinIOUploader.cpp)

find_package(unofficial-restbed CONFIG REQUIRED)
find_package(CURL REQUIRED)


target_link_libraries(Progetto PRIVATE unofficial::restbed::restbed)
target_link_libraries(Progetto PRIVATE ${CURL_LIBRARIES})
