cmake_minimum_required(VERSION 3.27)
project(
  psm
  VERSION 0.3.0
  DESCRIPTION "Color Space conversion library"
  HOMEPAGE_URL "https://github.com/neg-c/psm"
  LANGUAGES CXX)

set(PROJECT_VERSION_SUFFIX "alpha")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(NoInSourceBuilds)
include(Options)

add_subdirectory(src bin)
add_subdirectory(test)

include(Package)
include(Version)
include(Install)
