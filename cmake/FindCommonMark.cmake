include(FeatureSummary)
include(FindPackageHandleStandardArgs)
include(FindPackageMessage)

find_package(cmark QUIET CONFIG)

if (NOT TARGET cmark::cmark AND TARGET cmark::cmark_static)
  add_library(cmark::cmark ALIAS cmark::cmark_static)
endif()

# Include pkg-config compatibility for older versions
if(NOT TARGET cmark::cmark)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(cmark QUIET libcmark)
  endif()
  if(cmark_FOUND)
    add_library(cmark::cmark UNKNOWN IMPORTED)
    set_target_properties(cmark::cmark PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${cmark_INCLUDE_DIRS}"
      IMPORTED_LOCATION ${cmark_LINK_LIBRARIES}
    )
  endif()
endif()

if(TARGET cmark::cmark)
  get_target_property(cmark_LIBRARY cmark::cmark IMPORTED_LOCATION)

  if(NOT cmark_VERSION)
    get_target_property(cmark_INCLUDE_DIR cmark::cmark INTERFACE_INCLUDE_DIRECTORIES)
    find_file(cmark_HEADER "cmark_version.h" HINTS ${cmark_INCLUDE_DIR})
    file(STRINGS "${cmark_HEADER}" cmark_H REGEX "^#define CMARK_VERSION_STRING \"[^\"]*\"$")
    string(REGEX REPLACE "^.*CMARK_VERSION_STRING \"([0-9]+).*$" "\\1" cmark_VERSION_MAJOR "${cmark_H}")
    string(REGEX REPLACE "^.*CMARK_VERSION_STRING \"[0-9]+\\.([0-9]+).*$" "\\1" cmark_VERSION_MINOR  "${cmark_H}")
    string(REGEX REPLACE "^.*CMARK_VERSION_STRING \"[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" cmark_VERSION_PATCH "${cmark_H}")
    set(cmark_VERSION2 "${cmark_VERSION_MAJOR}.${cmark_VERSION_MINOR}.${cmark_VERSION_PATCH}")
    unset(cmark_H)
  endif()
endif()

find_package_handle_standard_args(CommonMark
  REQUIRED_VARS cmark_LIBRARY
  VERSION_VAR cmark_VERSION
)

set_package_properties(CommonMark PROPERTIES
    URL "https://github.com/commonmark/cmark"
    DESCRIPTION "CommonMark parsing and rendering library and program in C"
)
