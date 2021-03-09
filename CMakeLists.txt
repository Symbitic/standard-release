cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(CMAKE_BUILD_TYPE_INIT "Debug")

project(standard-release
    VERSION "0.1.0"
    DESCRIPTION "Automated workflows using conventional commits and semantic versioning"
    HOMEPAGE_URL "https://github.com/Symbitic/standard-release"
    LANGUAGES CXX
)

if(CMAKE_BUILD_TYPE MATCHES "[Dd]ebug")
    option(ENABLE_TESTS "Enable unit tests" ON)
else()
    option(ENABLE_TESTS "Enable unit tests" OFF)
endif()

option(BUILD_COMMONMARK "Build cmark instead of using system-wide version" OFF)
option(BUILD_LIBGIT2 "Build libgit2 instead of using system-wide version" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(FeatureSummary)
include(GNUInstallDirs)
include(InstallRequiredSystemLibraries)

add_subdirectory(src)

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
