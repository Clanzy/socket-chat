add_library(server server.hpp server.cpp )
add_library(client client.hpp client.cpp )

target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})