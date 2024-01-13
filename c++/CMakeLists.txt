cmake_minimum_required(VERSION 3.26)
project(Progetto)

set(CMAKE_CXX_STANDARD 20)

add_executable(Progetto
        socket/SocketTCP.h
        exceptions/SocketException.h
        minio-client/MinIOUploader.h
        minio-client/MinIOUploader.cpp
        database/MongoDB.cpp
        database/MongoDB.h
        test.cpp
        exceptions/LoginException.h
        handler/Handler.cpp
        handler/Handler.h
        server/Server.cpp
        server/Server.h
        main.cpp
        socket/SocketTCP.cpp
        exceptions/SignupException.h
        exceptions/SignupException.h
        exceptions/UploadException.h
        exceptions/MinioException.h)

find_package(unofficial-restbed CONFIG REQUIRED)
find_package(CURL REQUIRED)

find_package(unofficial-inih CONFIG REQUIRED)
target_link_libraries(Progetto PRIVATE unofficial::inih::inireader)

find_package(bsoncxx CONFIG REQUIRED)
target_link_libraries(Progetto PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)

find_package(mongocxx CONFIG REQUIRED)
target_link_libraries(Progetto PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)

find_package(cpp-jwt CONFIG REQUIRED)
target_link_libraries(Progetto PRIVATE cpp-jwt::cpp-jwt)

find_path(CPP_BASE64_INCLUDE_DIRS "cpp-base64/base64.cpp")
target_include_directories(Progetto PRIVATE ${CPP_BASE64_INCLUDE_DIRS})


find_package(AWSSDK CONFIG COMPONENTS core dynamodb kinesis s3 REQUIRED)
target_include_directories(Progetto PRIVATE ${AWSSDK_INCLUDE_DIRS})
target_link_libraries(Progetto PRIVATE ${AWSSDK_LIBRARIES})

target_link_libraries(Progetto PRIVATE unofficial::restbed::restbed)
target_link_libraries(Progetto PRIVATE ${CURL_LIBRARIES})
