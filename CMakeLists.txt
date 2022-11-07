cmake_minimum_required(VERSION 3.0.0)
project(chat VERSION 1.0.0)

include(CTest)
enable_testing()
set(CXX_STANDARD 20)

add_subdirectory(src)
add_subdirectory(test)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
