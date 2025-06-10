cmake_minimum_required(VERSION 3.12)

project(ChatConsoleNetDB)

set(CMAKE_CXX_STANDARD 20)  # Use C++17 or later
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(SERVER_SOURCE_DIR server)
set(CLIENT_SOURCE_DIR client)

file(GLOB S_SOURCES "${SERVER_SOURCE_DIR}/*.cpp")
file(GLOB S_HEADERS "${SERVER_SOURCE_DIR}/*.h")
file(GLOB C_SOURCES "${CLIENT_SOURCE_DIR}/*.cpp")
file(GLOB C_HEADERS "${CLIENT_SOURCE_DIR}/*.h")

# Add the executable for the server
add_executable(server ${S_SOURCES} ${S_HEADERS})

# Add the executable for the client
add_executable(client ${C_SOURCES} ${C_HEADERS})

# Linking external libraries
if(WIN32)
    set(ODBC_LIBRARIES odbc32)
    target_link_libraries(server ${ODBC_LIBRARIES})

    include_directories("C:/Program\ Files/MySQL/MySQL\ Server\ 8.0/include")
    target_link_libraries(server "C:/Program\ Files/MySQL/MySQL\ Server 8.0/lib/libmysql.lib")
    target_link_libraries(server wsock32 ws2_32)
    target_link_libraries(client wsock32 ws2_32)
endif()

if(UNIX)
    target_link_libraries(server /usr/lib64/libmysqlclient.so)
endif(UNIX)

# Configure include directories (headers)
if(WIN32)
    include_directories("C:/Program\ Files/MySQL/MySQL\ Server\ 8.0/include")
endif()

# Add other configuration or compiler flags as needed
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Install the executables (optional)
# install(TARGETS server client DESTINATION bin)
