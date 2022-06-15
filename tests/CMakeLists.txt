project(Google_tests)

add_subdirectory(./googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(.)

# TODO: is this needed?
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_executable(
        Google_Tests_run
        test_tabulated_data_inference.cpp
        test_parsing.cpp)

set_property(TARGET Google_Tests_run PROPERTY CXX_STANDARD 14)

target_link_libraries(
        Google_Tests_run PUBLIC gtest_main ${PROJECT_LIB_NAME} mpc
)
