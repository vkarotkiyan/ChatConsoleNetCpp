cmake_minimum_required(VERSION 3.10)

project(ChatConsoleNetDBClient VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()
