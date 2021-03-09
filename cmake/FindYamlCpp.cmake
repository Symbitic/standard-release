#.rst:
# FindYAMLCPP
# -----------
#
# Find YAML-CPP.
#
# This will define the following target:
#
# ``YAML-CPP::YAML-CPP``
#     YAML-CPP library.

include(FindPackageHandleStandardArgs)
include(FeatureSummary)

find_path(YAMLCPP_INCLUDE_DIR yaml-cpp/yaml.h)
find_library(YAMLCPP_LIBRARY NAMES libyaml-cpp.a libyaml-cpp.lib yaml-cpp)

find_package_handle_standard_args(YamlCpp
    FOUND_VAR
        YamlCpp_FOUND
    REQUIRED_VARS
        YAMLCPP_LIBRARY
        YAMLCPP_INCLUDE_DIR
)

if(YamlCpp_FOUND AND NOT TARGET YAML-CPP::YAML-CPP)
    add_library(YAML-CPP::YAML-CPP UNKNOWN IMPORTED)
    set_target_properties(YAML-CPP::YAML-CPP PROPERTIES
        IMPORTED_LOCATION "${YAMLCPP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${YAMLCPP_INCLUDE_DIR}"
    )
endif()

set_package_properties(YamlCpp PROPERTIES
    URL "https://github.com/jbeder/yaml-cpp"
    DESCRIPTION "A YAML parser and emitter in C++"
)
