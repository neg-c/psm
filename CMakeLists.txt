cmake_minimum_required(VERSION 3.27)
project(
  psm
  VERSION 1.0.0
  DESCRIPTION "Color Space conversion library"
  HOMEPAGE_URL "https://github.com/neg-c/psm"
  LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(NoInSourceBuilds)
include(Options)

add_subdirectory(src bin)
add_subdirectory(test)

include(Package)
include(Version)
include(Install)
