cmake_minimum_required(VERSION 3.26)
project(Progetto)

set(CMAKE_CXX_STANDARD 20)

add_executable(Progetto
        server.cpp
        SocketTCP.h
        exceptions/SocketException.h
        minio-client/MinIOUploader.h
        minio-client/MinIOUploader.cpp
        database/MongoDB.cpp
        database/MongoDB.h
        test.cpp
        exceptions/LoginException.h)

find_package(unofficial-restbed CONFIG REQUIRED)
find_package(CURL REQUIRED)

find_package(bsoncxx CONFIG REQUIRED)
target_link_libraries(Progetto PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)

find_package(mongocxx CONFIG REQUIRED)
target_link_libraries(Progetto PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)


target_link_libraries(Progetto PRIVATE unofficial::restbed::restbed)
target_link_libraries(Progetto PRIVATE ${CURL_LIBRARIES})
