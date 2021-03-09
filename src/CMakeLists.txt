include(GenerateExportHeader)

find_package(CommonMark REQUIRED)
find_package(LibGit2 REQUIRED)
find_package(CxxOpts REQUIRED)
find_package(YamlCpp REQUIRED)

add_library(StandardRelease
    standard-release/standard-release.cpp
    standard-release/standard-release.h
    standard-release/changelog/ichangelog.h
    standard-release/changelog/ichangelog.cpp
    standard-release/changelog/changelog.h
    standard-release/changelog/changelog.cpp
    standard-release/config/iconfig.cpp
    standard-release/config/iconfig.h
    standard-release/config/yaml.cpp
    standard-release/config/yaml.h
    standard-release/commits/conventional.cpp
    standard-release/commits/conventional.h
    standard-release/commits/iconventional.cpp
    standard-release/commits/iconventional.h
    standard-release/errors/errors.h
    standard-release/errors/errors.cpp
    standard-release/git/hooks.cpp
    standard-release/git/hooks.h
    standard-release/git/repository.cpp
    standard-release/git/repository.h
    standard-release/semver/semver.cpp
    standard-release/semver/semver.h
    standard-release/sources/isource.cpp
    standard-release/sources/isource.h
    standard-release/sources/json.cpp
    standard-release/sources/json.h
    standard-release/sources/text.cpp
    standard-release/sources/text.h
)

set_target_properties(StandardRelease PROPERTIES
    OUTPUT_NAME standard-release
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(StandardRelease
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/standard-release>
    PUBLIC
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(StandardRelease PRIVATE
    STANDARDRELEASE_VERSION=${PROJECT_VERSION}
)

target_link_libraries(StandardRelease PUBLIC
    LibGit2::LibGit2
    cmark::cmark
    cxxopts::cxxopts
    YAML-CPP::YAML-CPP
)

generate_export_header(StandardRelease
    EXPORT_FILE_NAME standard-release/global/global.h
)

add_executable(standard-release
    main.cpp
)

target_link_libraries(standard-release PRIVATE
    StandardRelease
)

install(TARGETS standard-release DESTINATION ${CMAKE_INSTALL_BINDIR})
