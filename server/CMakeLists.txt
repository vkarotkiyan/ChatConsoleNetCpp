cmake_minimum_required(VERSION 3.10)

project(ChatConsoleNetServer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp misc.h chatserver.h user.h message.h netserver.h misc.cpp chatserver.cpp user.cpp message.cpp netserver.cpp)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()
