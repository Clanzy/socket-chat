add_executable(serverapp serverapp.cpp server.cpp)
add_executable(clientapp clientapp.cpp client.cpp ui.cpp)

set(CXX_STANDARD 20)

find_package(Threads REQUIRED)
target_link_libraries(clientapp PRIVATE Threads::Threads ncurses)
target_link_libraries(serverapp PRIVATE Threads::Threads)
