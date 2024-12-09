cmake_minimum_required(VERSION 3.27)
project(
  psm
  VERSION 0.0.1
  DESCRIPTION "Color Space conversion library"
  HOMEPAGE_URL "https://github.com/neg-c/psm"
  LANGUAGES CXX)

set(PROJECT_VERSION_SUFFIX "alpha")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(NoInSourceBuilds)

add_subdirectory(src bin)

include(Package)
include(Version)
include(Install)
