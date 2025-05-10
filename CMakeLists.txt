cmake_minimum_required(VERSION 3.12)

project(ChatConsoleNet)

set(CMAKE_CXX_STANDARD 17)  # Use C++17 or later
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Add the executable for the server
add_executable(server
    server/main.cpp 
    server/misc.h
    server/chatserver.h
    server/user.h
    server/message.h
    server/netserver.h
    server/misc.cpp
    server/chatserver.cpp
    server/user.cpp
    server/message.cpp
    server/netserver.cpp
)

# Add the executable for the client
add_executable(client
    client/main.cpp 
    client/misc.h
    client/chatclient.h
    client/user.h
    client/message.h
    client/net.h
    client/misc.cpp
    client/chatclient.cpp
    client/user.cpp
    client/message.cpp
    client/net.cpp
)

# Linking external libraries
if(WIN32)
    target_link_libraries(server wsock32 ws2_32)
    target_link_libraries(client wsock32 ws2_32)
endif()

# Configure include directories (headers)

# Add other configuration or compiler flags as needed
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Install the executables (optional)
# install(TARGETS server client DESTINATION bin)
