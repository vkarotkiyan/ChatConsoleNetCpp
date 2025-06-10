cmake_minimum_required(VERSION 3.10)

project(ChatConsoleNetDBServer VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

if(WIN32)
    set(ODBC_LIBRARIES odbc32)
    target_link_libraries(${PROJECT_NAME} ${ODBC_LIBRARIES})

    include_directories("C:/Program\ Files/MySQL/MySQL\ Server\ 8.0/include")
    target_link_libraries(${PROJECT_NAME} "C:/Program\ Files/MySQL/MySQL\ Server 8.0/lib/libmysql.lib")
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()

if(UNIX)
    target_link_libraries(server /usr/lib64/libmysqlclient.so)
endif(UNIX)
