include(FetchContent)

FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.9.0
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)

include(Catch)

enable_testing()
add_executable(bitset_test test_reference.cpp
        test_subset.cpp
        test_inplace.cpp)
target_link_libraries(bitset_test PUBLIC bitset PRIVATE Catch2WithMain)
catch_discover_tests(bitset_test)