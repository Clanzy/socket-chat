add_executable(serverapp serverapp.cpp )
add_executable(clientapp clientapp.cpp )

set(CXX_STANDARD 20)

find_package(Threads REQUIRED)
target_link_libraries(clientapp PRIVATE Threads::Threads)
target_link_libraries(serverapp PRIVATE Threads::Threads)
