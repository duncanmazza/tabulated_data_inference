cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)

set(PROJECT_NAME TabulatedDataInference)
set(PROJECT_LIB_NAME lib${PROJECT_NAME})
set(HELPERS_LIB_NAME lib${PROJECT_NAME}Helpers)

project(iterate_file_insights)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
find_package(Boost REQUIRED)

include_directories(
        .
        include
        ${Boost_INCLUDE_DIRS}
        mpc
)

add_library(
        ${HELPERS_LIB_NAME} STATIC
        src/delim_helpers.cpp
        src/grammar.cpp
)

add_library(
        ${PROJECT_LIB_NAME} STATIC
        src/tabulated_data_inference.cpp
)

add_library(
        mpc
        mpc/mpc.c
)
set_property(TARGET mpc PROPERTY C_STANDARD 11)

target_link_libraries(
        ${PROJECT_LIB_NAME} PUBLIC ${HELPERS_LIB_NAME} mpc
)

add_executable(
        example
        example_script.cpp
)
target_link_libraries(example PRIVATE ${PROJECT_LIB_NAME} ${Boost_LIBS})

add_subdirectory(tests)
