add_executable(chat_test test.cpp)

find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)

target_link_libraries(chat_test
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      )

                      
target_include_directories(chat_test PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_test(NAME test1 COMMAND chat_test)