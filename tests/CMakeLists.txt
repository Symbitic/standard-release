include(FetchContent)

FetchContent_Declare(ut
    GIT_REPOSITORY https://github.com/boost-ext/ut.git
    #GIT_TAG        v1.1.7
)

set(ut_BUILD_BENCHMARKS OFF)
set(ut_BUILD_EXAMPLES OFF)
set(ut_BUILD_TESTS OFF)

FetchContent_GetProperties(ut)
if(NOT ut_POPULATED)
  FetchContent_Populate(ut)
  add_subdirectory(${ut_SOURCE_DIR} ${ut_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

foreach(name IN ITEMS semver changelog)
    add_executable(test_${name} "test_${name}.cpp")
    set_target_properties(test_${name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )

    target_compile_definitions(test_${name} PRIVATE
        FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures/"
    )

    # boost::ut with next release.
    target_link_libraries(test_${name} PRIVATE StandardRelease boost::ut)

    add_test(NAME test_${name} COMMAND test_${name})
endforeach()
