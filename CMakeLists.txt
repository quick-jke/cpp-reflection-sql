cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
project(testapp)

add_custom_target(copy_models ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/models
    COMMENT "Copying models folder to build directory"
)
add_subdirectory(utils)
add_subdirectory(scanner)
add_subdirectory(sql)


add_executable(testapp app/main.cpp)
target_include_directories(testapp PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(testapp PUBLIC Scanner Sql)

