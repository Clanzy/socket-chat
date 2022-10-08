add_executable(serverapp serverapp.cpp )
add_executable(clientapp clientapp.cpp )

target_link_libraries(serverapp PRIVATE server)
target_link_libraries(clientapp PRIVATE client)