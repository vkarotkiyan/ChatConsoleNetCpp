cmake_minimum_required(VERSION 3.10)

project(ChatConsoleNetClient VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp misc.h chatclient.h user.h message.h net.h misc.cpp chatclient.cpp user.cpp message.cpp net.cpp)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()
